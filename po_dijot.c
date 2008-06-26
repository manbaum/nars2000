//***************************************************************************
//  NARS2000 -- Primitive Operator -- DieresisJot
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
//  $PrimOpDieresisJot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator DieresisJot ("rank" and "rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDieresisJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DIERESISJOT
         || lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_DIERESISJOT);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDieresisJot_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg);     // Ptr to right arg token
    else
        return PrimOpDydDieresisJot_EM_YY (lptkLftArg,      // Ptr to left arg token
                                           lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg);     // Ptr to right arg token
} // End PrimOpDieresisJot_EM_YY


//***************************************************************************
//  $PrimProtoOpDieresisJot_EM_YY
//
//  Generate a prototype for the derived functions from
//    dyadic operator DieresisJot ("rank" and "rank")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpDieresisJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoOpDieresisJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    if (lptkAxis EQ NULL)
        // Check for axis operator
        lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto SYNTAX_EXIT;

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimOpMonDieresisJotCommon_EM_YY (lpYYFcnStrOpr,     // Ptr to operator function strand
                                                 lptkRhtArg,        // Ptr to right arg token
                                                 TRUE);             // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydDieresisJotCommon_EM_YY (lptkLftArg,        // Ptr to left arg token
                                                 lpYYFcnStrOpr,     // Ptr to operator function strand
                                                 lptkRhtArg,        // Ptr to right arg token
                                                 TRUE);             // TRUE iff prototyping
SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimProtoOpDieresisJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonDieresisJot_EM_YY
//
//  Primitive operator for monadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresisJot_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpMonDieresisJotCommon_EM_YY (lpYYFcnStrOpr,     // Ptr to operator function strand
                                             lptkRhtArg,        // Ptr to right arg token
                                             FALSE);            // TRUE iff prototyping
} // End PrimOpDieresisJot_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisJotCommon_EM_YY
//
//  Primitive operator for monadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresisJotCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (NULL NE CheckAxisOper (lpYYFcnStrOpr))];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->TknCount];

    return
      PrimOpDieresisJotCommon_EM_YY (NULL,              // Ptr to left arg token (may be NULL if monadic derived function)
                                     lpYYFcnStrLft,     // Ptr to left operand function strand
                                     lpYYFcnStrOpr,     // Ptr to operator function strand
                                     lpYYFcnStrRht,     // Ptr to right operand function strand
                                     lptkRhtArg,        // Ptr to right arg token
                                     TRUE,              // TRUE iff check for axis operator
                                     bPrototyping);     // TRUE iff protoyping
} // End PrimOpMonDieresisJotCommon_EM_YY


//***************************************************************************
//  $PrimOpDieresisJotCommon_EM_YY
//
//  Primitive operator for monadic & dyadic derived function
//    from DieresisJot ("rank")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDieresisJotCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpDieresisJotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic derived function)
     LPPL_YYSTYPE lpYYFcnStrLft,        // Ptr to left operand function strand
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPPL_YYSTYPE lpYYFcnStrRht,        // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bCheckAxis,           // TRUE iff check for axis operator
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    HGLOBAL      hGlbPTD,               // PerTabData global memory handle
                 hGlbMF1,               // Magic function #1 global memory handle
                 hGlbMF2,               // Magic function #2 global memory handle
                 hGlbOprRht;            // Right operand global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to result
    LPTOKEN      lptkAxis;              // Ptr to axis token

    // Check for axis operator if requested to do so
    if (bCheckAxis)
    {
        lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

        //***************************************************************
        // The derived functions from this operator are not sensitive
        //   to the axis operator, so signal a syntax error if present
        //***************************************************************
        if (lptkAxis NE NULL)
            goto SYNTAX_EXIT;
    } // End IF

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken))
        goto LEFT_SYNTAX_EXIT;

    // Ensure the right operand is a variable
    if (IsTknFcnOpr (&lpYYFcnStrRht->tkToken))
        goto RIGHT_SYNTAX_EXIT;

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the magic function global memory handles
    hGlbMF1 = (lptkLftArg EQ NULL) ? lpMemPTD->hGlbMF_MonRank
                                   : lpMemPTD->hGlbMF_DydRank;
    hGlbMF2 = lpMemPTD->hGlbMF_Conform;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Get right operand global ptrs
    hGlbOprRht = GetGlbHandle (&lpYYFcnStrRht->tkToken);

    // If the right operand is not immediate, ...
    if (hGlbOprRht NE NULL)
    {
        APLSTYPE aplTypeOprRht;             // Right operand storage type
        APLNELM  aplNELMOprRht;             // Right operand NELM
        APLRANK  aplRankOprRht;             // Right operand rank

        // Get the attributes (Type, NELM, and Rank)
        //   of the right arg token
        AttrsOfGlb (hGlbOprRht, &aplTypeOprRht, &aplNELMOprRht, &aplRankOprRht, NULL);

        // Check for RIGHT OPERAND RANK ERROR
        if (1 < aplRankOprRht)
            goto RANK_EXIT;

        // Check for RIGHT OPERAND LENGTH ERROR
        if (1 > aplNELMOprRht
         ||     aplNELMOprRht > 3)
            goto LENGTH_EXIT;

        // Check for RIGHT OPERAND DOMAIN ERROR
        if (!IsSimpleNum (aplTypeOprRht))
            goto DOMAIN_EXIT;
    } else
    // Check for RIGHT OPERAND DOMAIN ERROR
    if (!IsImmNum (lpYYFcnStrRht->tkToken.tkFlags.ImmType))
        goto DOMAIN_EXIT;

    lpYYRes =
      ExecuteMagicOperator_EM_YY (lptkLftArg,               // Ptr to left arg token
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                  lpYYFcnStrLft,            // Ptr to left operand function strand
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  lpYYFcnStrRht,            // Ptr to right operand function strand
                                  lptkRhtArg,               // Ptr to right arg token
                                  NULL,                     // Ptr to axis token
                                  hGlbMF1,                  // Magic function global memory handle
                                  bPrototyping
                                ? LINENUM_PROTOTYPE
                                : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
    // If the result is valid, finish the process with Conforming Disclose
    if (lpYYRes && !IsTknImmed (lpYYRes->tkToken.tkFlags.TknType))
    {
        HGLOBAL     hGlbRes;                // Result global memory handle
        LPAPLNESTED lpMemRes;               // Ptr to result global memory
        APLSTYPE    aplTypeRes;             // Result storage type
        APLNELM     aplNELMRes;             // Result NELM
        APLRANK     aplRankRes;             // Result rank

        // Get result global ptrs
        hGlbRes = GetGlbHandle (&lpYYRes->tkToken);

        // Get the attributes (Type, NELM, and Rank)
        //   of the result global
        AttrsOfGlb (hGlbRes, &aplTypeRes, &aplNELMRes, &aplRankRes, NULL);

        // If the result is not simple, ...
        if (!IsSimple (aplTypeRes))
        {
            APLRANK      uMinRank,          // Minimum rank across all items
                         uMaxRank;          // maximum ...
            APLUINT      uRes;              // Loop counter
            APLRANK      aplRankItm;        // Result item rank
            LPPL_YYSTYPE lpYYRes2;          // Secondary result

            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

            // Skip over the header to the data
            lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

            // Initialize minimum & maximum ranks
            uMinRank = NEG1A;
            uMaxRank = 0;

            // Loop through the result looking for the minimum/maximum rank items
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            // Split cases based the item ptr type
            switch (GetPtrTypeDir (lpMemRes[uRes]))
            {
                case PTRTYPE_STCONST:
                    // Get the minimum rank
                    uMinRank = min (uMinRank, 0);

                    break;

                case PTRTYPE_HGLOBAL:
                    // Get the attributes (Type, NELM, and Rank)
                    //   of the result global item
                    AttrsOfGlb (ClrPtrTypeDirAsGlb (lpMemRes[uRes]), NULL, NULL, &aplRankItm, NULL);

                    // Get the minimum rank
                    uMinRank = min (uMinRank, aplRankItm);

                    // Get the maximum rank
                    uMaxRank = max (uMaxRank, aplRankItm);

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // If the two ranks are unequal, ...
            if (uMinRank NE uMaxRank)
            {
                TOKEN tkLftArg = {0};   // Left arg token

                // Fill in the left arg token
                tkLftArg.tkFlags.TknType   = TKT_VARIMMED;
                tkLftArg.tkFlags.ImmType   = IMMTYPE_INT;
////////////////tkLftArg.tkFlags.NoDisplay = FALSE; // Already zero from = {0}
                tkLftArg.tkData.tkInteger  = uMaxRank;
////////////////tkLftArg.tkCharIndex       = 0;     // Already zero from = {0}
#ifdef DEBUG
                // Decrement the SI level of lpYYRes so YYResIsEmpty won't complain
                lpYYRes->SILevel--;
#endif
                // Finish it off with another magic function
                lpYYRes2 =
                  ExecuteMagicFunction_EM_YY (&tkLftArg,                // Ptr to left arg token
                                              &lpYYFcnStrOpr->tkToken,  // Ptr to function token
                                               NULL,                    // Ptr to function strand
                                              &lpYYRes->tkToken,        // Ptr to right arg token
                                               NULL,                    // Ptr to axis token
                                               hGlbMF2,                 // Magic function global memory handle
                                               LINENUM_ONE);            // Starting line # type (see LINE_NUMS)
#ifdef DEBUG
                // Restore the SI level of lpYYRes
                lpYYRes->SILevel++;
#endif
            } else
                // Just disclose the argument
                lpYYRes2 =
                  PrimFnMonRightShoe_EM_YY (&lpYYFcnStrOpr->tkToken,    // Ptr to function token
                                            &lpYYRes->tkToken,          // Ptr to right arg token
                                             NULL);                     // Ptr to axis token
            // Free the first YYRes
            FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;

            // Copy secondary result as primary result
            lpYYRes = lpYYRes2;
        } // End IF
    } // End IF

    return lpYYRes;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    return NULL;

RIGHT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    return NULL;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    return NULL;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    return NULL;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    return NULL;
} // End PrimOpDieresisJotCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function for monadic derived function from the rank dyadic operator
//***************************************************************************

static APLCHAR MonHeader[] =
  $Z $IS L"(" $LO L" " $F L" " $X L") " $R L";" $O;

static APLCHAR MonLine1[] =
  $X $IS L"1" $RHO $X;

static APLCHAR MonLine2[] =
  $O $IS $RHO $RHO $R;

static APLCHAR MonLine3[] =
  $X $IS L"(-" $O L")" $MAX $O $MIN $X;

static APLCHAR MonLine4[] =
//$Z $IS $DISCLOSE $LO $EACH $ENCLOSE L"[" $IOTA L"-" $X L"]" $R
  $Z $IS           $LO $EACH $ENCLOSE L"[" $IOTA L"-" $X L"]" $R
  $DIAMOND $GOTO L"0";

static APLCHAR MonLine5[] =
  $QUAD_PROTOTYPE L":"
//$Z $IS $DISCLOSE $DISCLOSE $LO $EACH $EACH $ENCLOSE L"[" $IOTA L"-" $X L"]" $EACH L"0" $RHO $ENCLOSE $R;
  $Z $IS           $DISCLOSE $LO $EACH $EACH $ENCLOSE L"[" $IOTA L"-" $X L"]" $EACH L"0" $RHO $ENCLOSE $R;

static LPAPLCHAR MonBody[] =
{MonLine1,
 MonLine2,
 MonLine3,
 MonLine4,
 MonLine5,
};

MAGIC_FUNCTION MF_MonRank =
{MonHeader,
 MonBody,
 sizeof (MonBody) / sizeof (MonBody[0]),
};


static APLCHAR ConHeader[] =
  $Z $IS $L L" " $F L" " $R;

static APLCHAR ConLine1[] =
  $Z $IS $DISCLOSE L"(((" $L L"-" $EPSILON $RHO $JOT $RHO $EACH $R L")" $RHO $EACH L"1" L")," $EACH $RHO $EACH $R L")" $RHO $EACH $R;

static LPAPLCHAR ConBody[] =
{ConLine1,
};

MAGIC_FUNCTION MF_Conform =
{ConHeader,
 ConBody,
 sizeof (ConBody) / sizeof (ConBody[0]),
};


//***************************************************************************
//  $PrimOpDydDieresisJot_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydDieresisJotCommon_EM_YY (lptkLftArg,    // Ptr to left arg token
                                             lpYYFcnStrOpr, // Ptr to operator function strand
                                             lptkRhtArg,    // Ptr to right arg token
                                             FALSE);        // TRUE iff prototyping
} // End PrimOpDydDieresisJot_EM_YY


//***************************************************************************
//  $PrimOpDydDieresisJotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisJotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (NULL NE CheckAxisOper (lpYYFcnStrOpr))];
    lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->TknCount];

    return
      PrimOpDieresisJotCommon_EM_YY (lptkLftArg,        // Ptr to left arg token (may be NULL if monadic derived function)
                                     lpYYFcnStrLft,     // Ptr to left operand function strand
                                     lpYYFcnStrOpr,     // Ptr to operator function strand
                                     lpYYFcnStrRht,     // Ptr to right operand function strand
                                     lptkRhtArg,        // Ptr to right arg token
                                     TRUE,              // TRUE iff check for axis operator
                                     bPrototyping);     // TRUE iff protoyping
} // End PrimOpDydDieresisJotCommon_EM_YY


//***************************************************************************
//  Magic function for dyadic derived function from the rank dyadic operator
//***************************************************************************

static APLCHAR DydHeader[] =
  $Z $IS $L L" (" $LO L" " $F L" " $X L") " $R L";" $O;

static APLCHAR DydLine1[] =
  $X $IS L"1" $DROP $REVERSE L"3" $RHO $REVERSE $X;

static APLCHAR DydLine2[] =
  $O $IS L"(" $RHO $RHO $L L")," $RHO $RHO $R;

static APLCHAR DydLine3[] =
  $X $IS L"(-" $O L")" $MAX $O $MIN $X;

static APLCHAR DydLine4[] =
  $Z $IS L"(" $ENCLOSE L"[" $IOTA L"-1" $TAKE $X L"]" $L L")"
         $LO $EACH
              $ENCLOSE L"[" $IOTA L"-1" $DROP $X L"]" $R
  $DIAMOND $GOTO L"0";

static APLCHAR DydLine5[] =
  $QUAD_PROTOTYPE L":"
  $Z $IS $DISCLOSE L"(" $ENCLOSE L"[" $IOTA L"-1" $TAKE $X L"]" $EACH L"0" $RHO $ENCLOSE $L L")"
                   $LO $EACH $EACH
                        $ENCLOSE L"[" $IOTA L"-1" $DROP $X L"]" $EACH L"0" $RHO $ENCLOSE $R;

static LPAPLCHAR DydBody[] =
{DydLine1,
 DydLine2,
 DydLine3,
 DydLine4,
 DydLine5,
};

MAGIC_FUNCTION MF_DydRank =
{DydHeader,
 DydBody,
 sizeof (DydBody) / sizeof (DydBody[0]),
};


//***************************************************************************
//  End of File: po_dieresisjot.c
//***************************************************************************
