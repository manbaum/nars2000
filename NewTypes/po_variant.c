//***************************************************************************
//  NARS2000 -- Primitive Operator -- Variant
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_linalg.h>


VARIANTKEYSTR aVariantKeyStr[VARIANT_KEY_LENGTH]
 = {
    {L"CT"      , SYSVAR_CT      , ValidSetCT_EM      },    // 00:  []CT
    {L"DT"      , SYSVAR_DT      , ValidSetDT_EM      },    // 01:  []DT
    {L"FPC"     , SYSVAR_FPC     , ValidSetFPC_EM     },    // 02:  []FPC
    {L"IO"      , SYSVAR_IO      , ValidSetIO_EM      },    // 03:  []IO
    {L"LR"      , SYSVAR_LR      , ValidSetLR_EM      },    // 04:  []LR
    {L"PP"      , SYSVAR_PP      , ValidSetPP_EM      },    // 05:  []PP
    {L"RL"      , SYSVAR_RL      , ValidSetRL_EM      },    // 06:  []RL
  };

// N.B.:  Whenever changing the above struct (aVariantKeyStr)
//   be sure to make a corresponding change to
//   <VARIANTKEYS> in <po_variant.h>.


typedef UBOOL (*VAROPR) (LPVOID);

// Table of Variant operator validation routines
VAROPR varOprValTab[ENUM_VARIANT_LENGTH] =
{
    NULL        ,                   // 00:  No value
    varOprCT    ,                   // 01:  []CT
    varOprDT    ,                   // 02:  []DT
    varOprFPC   ,                   // 03:  []FPC
    varOprIO    ,                   // 04:  []IO
    varOprLR    ,                   // 05:  []LR
    varOprPP    ,                   // 06:  []PP
    varOprRL    ,                   // 07:  []RL
    varOprEIG   ,                   // 08:  {domino} Eigenvalues, vectors, and Schur vectors
    varOprPOCH  ,                   // 09:  {shriek} Pochhammer k-symbol
    varOprHCM   ,                   // 0A:  {times}  Interior/Exterior/Cross/Dot/Conjugation product
    varOprSUB   ,                   // 0B:  {iota}   Ascending/Descending subsequence
    varOprGALL  ,                   // 0C:  {grade}  Grade all arrays
};

// N.B.:  Whenever changing the above struct (varOprValTab)
//   be sure to make a corresponding change to
//   <varOprType> in <po_variant.c> and
//   <ENUM_VARIANT> in <po_variant.h>.



APLSTYPE varOprType[ENUM_VARIANT_LENGTH] =
{
    ARRAY_ERROR ,                   // 00:  No value
    ARRAY_FLOAT ,                   // 01:  []CT
    ARRAY_CHAR  ,                   // 02:  []DT
    ARRAY_INT   ,                   // 03:  []FPC
    ARRAY_BOOL  ,                   // 04:  []IO
    ARRAY_CHAR  ,                   // 05:  []LR
    ARRAY_INT   ,                   // 06:  []PP
    ARRAY_INT   ,                   // 07:  []RL
    ARRAY_INT   ,                   // 08:  {domino} Eigenvalues, vectors, and Schur vectors
    ARRAY_INT   ,                   // 09:  {shriek} Pochhammer k-symbol
    ARRAY_CHAR  ,                   // 0A:  {times}  Interior/Exterior/Cross/Dot/Conjugation product
    ARRAY_CHAR  ,                   // 0B:  {iota}   Ascending/Descending subsequence
    ARRAY_INT   ,                   // 0C:  {grade}  Grade all arrays
};

// N.B.:  Whenever changing the above struct (varOprValType)
//   be sure to make a corresponding change to
//   <varOprValTab> in <po_variant.c> and
//   <ENUM_VARIANT> in <po_variant.h>.


LPVARIANT_STR varOprTab[PRIMTAB_LEN];   // The jump table for all Variant operator possibilities

VARIANT_STR varOprStr[] =
//                                ========Monadic=======|========Dyadic=========|==xxxTrans==
  {                         //    N1    N2    C1    C2    N1    N2    C1     C2
   VAR_MAC (CIRCLE              , UNK , UNK , UNK , UNK , UNK , UNK , LR   , UNK , FcnTrans),   // CIRCLE
   VAR_MAC (CIRCLESLOPE         , UNK , UNK , UNK , UNK , IO  , UNK , UNK  , UNK , FcnTrans),   // CIRCLESLOPE
   VAR_MAC (COLONBAR            , UNK , UNK , UNK , UNK , UNK , UNK , LR   , UNK , OprTrans),   // COLONBAR
   VAR_MAC (DELSTILE            , IO  , UNK , GALL, UNK , IO  , UNK , UNK  , UNK , FcnTrans),   // DELSTILE
   VAR_MAC (DELTASTILE          , IO  , UNK , GALL, UNK , IO  , UNK , UNK  , UNK , FcnTrans),   // DELTASTILE
   VAR_MAC (DOMINO              , EIG , UNK , UNK , UNK , UNK , UNK , UNK  , UNK , FcnTrans),   // DOMINO
   VAR_MAC (DOT                 , UNK , UNK , UNK , UNK , UNK , UNK , LR   , UNK , OprTrans),   // DOT
   VAR_MAC (DOUBLESHRIEK        , IO  , UNK , UNK , UNK , UNK , UNK , UNK  , UNK , OprTrans),   // DOUBLESHRIEK
   VAR_MAC (DOWNCARET           , UNK , UNK , UNK , UNK , UNK , UNK , LR   , UNK , FcnTrans),   // DOWNCARET
   VAR_MAC (DOWNSHOE            , CT  , UNK , UNK , UNK , UNK , UNK , UNK  , UNK , FcnTrans),   // DOWNSHOE
   VAR_MAC (DOWNSTILE           , CT  , UNK , UNK , UNK , UNK , UNK , UNK  , UNK , FcnTrans),   // DOWNSTILE
   VAR_MAC (DOWNTACK            , UNK , UNK , UNK , UNK , UNK , UNK , LR   , UNK , FcnTrans),   // DOWNTACK
   VAR_MAC (DOWNTACKJOT         , PP  , UNK , UNK , UNK , PP  , UNK , UNK  , UNK , FcnTrans),   // DOWNTACKJOT
   VAR_MAC (EPSILON             , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // EPSILON
   VAR_MAC (EPSILONUNDERBAR     , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // EPSILONUNDERBAR
   VAR_MAC (EQUAL               , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // EQUAL
   VAR_MAC (EQUALUNDERBAR       , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // EQUALUNDERBAR
   VAR_MAC (IOTA                , IO  , UNK , UNK , UNK , IO  , CT  , SUB  , UNK , FcnTrans),   // IOTA
   VAR_MAC (IOTAUNDERBAR        , IO  , UNK , UNK , UNK , IO  , CT  , UNK  , UNK , FcnTrans),   // IOTAUNDERBAR
   VAR_MAC (LEFTCARET           , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // LEFTCARET
   VAR_MAC (LEFTCARETUNDERBAR   , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // LEFTCARETUNDERBAR
   VAR_MAC (LEFTCARETUNDERBAR2  , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // LEFTCARETUNDERBAR2
   VAR_MAC (LEFTSHOE            , UNK , UNK , UNK , UNK , IO  , UNK , UNK  , UNK , FcnTrans),   // LEFTSHOE
   VAR_MAC (LEFTSHOEUNDERBAR    , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // LEFTSHOEUNDERBAR
   VAR_MAC (NOTEQUAL            , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // NOTEQUAL
   VAR_MAC (NOTEQUALUNDERBAR    , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // NOTEQUALUNDERBAR
   VAR_MAC (PI                  , UNK , UNK , UNK , UNK , IO  , UNK , UNK  , UNK , FcnTrans),   // PI
   VAR_MAC (QUADJOT             , CT  , UNK , LR  , UNK , CT  , UNK , LR   , UNK , OprTrans),   // QUADJOT
   VAR_MAC (QUERY               , IO  , RL  , DT  , UNK , IO  , RL  , DT   , UNK , FcnTrans),   // QUERY
   VAR_MAC (QUOTEDOT            , POCH, POCH, UNK , UNK , UNK , UNK , UNK  , UNK , OprTrans),   // QUOTEDOT
   VAR_MAC (RIGHTCARET          , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // RIGHTCARET
   VAR_MAC (RIGHTCARETUNDERBAR  , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // RIGHTCARETUNDERBAR
   VAR_MAC (RIGHTCARETUNDERBAR2 , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // RIGHTCARETUNDERBAR2
   VAR_MAC (RIGHTSHOEUNDERBAR   , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // RIGHTSHOEUNDERBAR
   VAR_MAC (SECTION             , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // SECTION
   VAR_MAC (SQUAD               , UNK , UNK , UNK , UNK , IO  , UNK , UNK  , UNK , FcnTrans),   // SQUAD
   VAR_MAC (STILE               , UNK , UNK , UNK , UNK , CT  , UNK , LR   , UNK , FcnTrans),   // STILE
   VAR_MAC (STILE2              , UNK , UNK , UNK , UNK , CT  , UNK , LR   , UNK , FcnTrans),   // STILE2
   VAR_MAC (TILDE               , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // TILDE
   VAR_MAC (TILDE2              , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // TILDE2
   VAR_MAC (TIMES               , UNK , UNK , UNK , UNK , UNK , UNK , HCM  , UNK , FcnTrans),   // TIMES
   VAR_MAC (UPCARET             , UNK , UNK , UNK , UNK , UNK , UNK , LR   , UNK , FcnTrans),   // UPCARET
   VAR_MAC (UPSHOE              , UNK , UNK , UNK , UNK , CT  , UNK , UNK  , UNK , FcnTrans),   // UPSHOE
   VAR_MAC (UPSTILE             , CT  , UNK , UNK , UNK , UNK , UNK , UNK  , UNK , FcnTrans),   // UPSTILE
   VAR_MAC (UPTACKJOT           , FPC , UNK , UNK , UNK , UNK , UNK , UNK  , UNK , FcnTrans),   // UPTACKJOT
  };

#define MAX_VAR_NUMLEN  2                                   // Maximum # numerics allowed in the right operand
#define MAX_VAR_CHRLEN  2                                   // ...       chars    ...
#define MAX_VAR_LEN     (MAX_VAR_NUMLEN + MAX_VAR_CHRLEN)   // Maximum # items allowed in the right operand


//***************************************************************************
//  $PrimOpVariant_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Variant ("variant" and "variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpVariant_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_VARIANT);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return
          PrimOpMonVariant_EM_YY (lpYYFcnStrOpr,        // Ptr to operator function strand
                                  lptkRhtArg);          // Ptr to right arg token
    else
        return
          PrimOpDydVariant_EM_YY (lptkLftArg,           // Ptr to left arg token
                                  lpYYFcnStrOpr,        // Ptr to operator function strand
                                  lptkRhtArg);          // Ptr to right arg token
} // End PrimOpVariant_EM_YY


//***************************************************************************
//  $PrimProtoOpVariant_EM_YY
//
//  Generate a prototype for the derived functions from
//    dyadic operator Variant ("variant" and "variant")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpVariant_EM_YY
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
        goto AXIS_SYNTAX_EXIT;

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return
          PrimOpMonVariantCom_EM_YY (lpYYFcnStrOpr,     // Ptr to operator function strand
                                     lptkRhtArg,        // Ptr to right arg token
                                     TRUE);             // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return
          PrimOpDydVariantCom_EM_YY (lptkLftArg,        // Ptr to left arg token
                                     lpYYFcnStrOpr,     // Ptr to operator function strand
                                     lptkRhtArg,        // Ptr to right arg token
                                     TRUE);             // TRUE iff prototyping
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimProtoOpVariant_EM_YY


//***************************************************************************
//  $PrimIdentOpVariant_EM_YY
//
//  Generate an identity element for the dyadic derived function from the
//    primitive operator QuadColon
//***************************************************************************

LPPL_YYSTYPE PrimIdentOpVariant_EM_YY
    (LPTOKEN      lptkRhtOrig,      // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,    // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)      // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYFcnStrLft,     // Ptr to left operand function strand
                 lpYYFcnStrRht;     // Ptr to right ...
    LPPRIMFLAGS  lpPrimFlagsLft;    // Ptr to left operand primitive flags

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig   NE NULL);
    Assert (lpYYFcnStrOpr NE NULL);
    Assert (lptkRhtArg    NE NULL);

    //***************************************************************
    // This operator is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (left) identity function for dyadic Variant
    //   (L f {variant} R) ("scalar op") is
    //   ({primops} f) R.

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, lptkAxisOpr               ); Assert (lpYYFcnStrRht NE NULL);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrOpr, lptkAxisOpr, lpYYFcnStrRht); Assert (lpYYFcnStrLft NE NULL);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Ensure the right operand is a variable
    if (IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Get the appropriate primitive flags ptr
    lpPrimFlagsLft = GetPrimFlagsPtr (&lpYYFcnStrLft->tkToken);

    // Check for error
    if (lpPrimFlagsLft EQ NULL || lpPrimFlagsLft->lpPrimOps EQ NULL)
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Execute the left operand identity function on the right arg
    return
      PrimOpVariantCom_EM_YY (lptkRhtOrig,              // Ptr to left arg token (may be NULL if monadic derived function)
                                                        //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                                        //     then this value is the original right arg
                              lpYYFcnStrLft,            // Ptr to left operand function strand
                              lpPrimFlagsLft,           // Ptr to left operand primitive flags (may be NULL)
                              lpYYFcnStrOpr,            // Ptr to operator function strand
                              lpYYFcnStrRht,            // Ptr to right operand function strand
                              lptkRhtArg,               // Ptr to right arg token
                              FALSE);                   // TRUE iff protoyping
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End PrimIdentOpVariant_EM_YY


//***************************************************************************
//  $PrimOpMonVariant_EM_YY
//
//  Primitive operator for monadic derived function from Variant ("variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonVariant_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return
      PrimOpMonVariantCom_EM_YY (lpYYFcnStrOpr,     // Ptr to operator function strand
                                 lptkRhtArg,        // Ptr to right arg token
                                 FALSE);            // TRUE iff prototyping
} // End PrimOpMonVariant_EM_YY


//***************************************************************************
//  $PrimOpMonVariantCom_EM_YY
//
//  Primitive operator for monadic derived function from Variant ("variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonVariantCom_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, CheckAxisOper (lpYYFcnStrOpr)               ); Assert (lpYYFcnStrRht NE NULL);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrOpr, CheckAxisOper (lpYYFcnStrOpr), lpYYFcnStrRht); Assert (lpYYFcnStrLft NE NULL);

    return
      PrimOpVariantCom_EM_YY (NULL,                     // Ptr to left arg token (may be NULL if monadic derived function)
                                                        //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                                        //     then this value is the original right arg
                              lpYYFcnStrLft,            // Ptr to left operand function strand
                              NULL,                     // Ptr to left operand primitive flags (may be NULL)
                              lpYYFcnStrOpr,            // Ptr to operator function strand
                              lpYYFcnStrRht,            // Ptr to right operand function strand
                              lptkRhtArg,               // Ptr to right arg token
                              bPrototyping);            // TRUE iff protoyping
} // End PrimOpMonVariantCom_EM_YY


//***************************************************************************
//  $PrimOpVariantCom_EM_YY
//
//  Primitive operator for monadic & dyadic derived function
//    from Variant ("variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpVariantCom_EM_YY
    (LPTOKEN      lptkLftArg,               // Ptr to left arg token (may be NULL if monadic derived function)
                                            //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                            //     then this value is the original right arg
     LPPL_YYSTYPE lpYYFcnStrLft,            // Ptr to left operand function strand
     LPPRIMFLAGS  lpPrimFlagsLft,           // Ptr to left operand primitive flags
     LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPPL_YYSTYPE lpYYFcnStrRht,            // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic derived function)
     UBOOL        bPrototyping)             // TRUE iff protoyping

{
    LPPL_YYSTYPE        lpYYRes = NULL;                 // Ptr to the result
    LPTOKEN             lptkAxisLft,                    // Ptr to axis token on the left operand
                        lptkAxisOpr;                    // Ptr to axis token on the operator
    APLSTYPE            aplTypeLft,                     // Left arg storage type
                        aplTypeRhtOpr,                  // Right operand storage type
                        aplTypeRht;                     // Right arg     ...
    APLNELM             aplNELMLft,                     // Right arg NELM
                        aplNELMRhtOpr,                  // Right operand NELM
                        uMaxLen,                        // Maximum length of the right operand
                        aplNELMRht;                     // Right arg     ...
    APLUINT             uRhtOpr,                        // Loop counter
                        uCnt,                           // ...
                        uNum,                           // ...
                        uChr;                           // ...
    APLRANK             aplRankLft,                     // Lftt arg rank
                        aplRankRhtOpr,                  // Right operand rank
                        aplRankRht;                     // Right arg     ...
    APLDIM              aplColsRht;                     // Right arg # cols
    APLLONGEST          aplLongestRhtOpr,               // Right operand value as an immediate
                        aplRhtOprItm[MAX_VAR_LEN];      // ...           item array
    UBOOL               bRet = TRUE,                    // TRUE iff the result is valid
                        bInitEnumVars = TRUE;           // TRUE iff the <lpEnumVarXX>s have neen initialized
    ENUM_NCF            eFound[MAX_VAR_LEN] = {ENUM_NCF_UNK};   // Array found state:  see ENUM_NCF
    HGLOBAL             hGlbRhtOpr = NULL;              // Right operand global memory handle
    LPVARARRAY_HEADER   lpMemHdrRhtOpr = NULL;          // ...    right operand ...
    LPVOID              lpMemRhtOpr;                    // Ptr to right operand memory
    HGLOBAL             hGlbMFO;                        // Magic function/operator global memory handle
    LPPERTABDATA        lpMemPTD;                       // Ptr to PerTabData global memory
    LPVARIANT_STR       lpVarOprStr;                    // Ptr to Variant operator struc
    ALLSYSVARS_STR      allSysVarsStr = {0};            // Save area for all System Vars
    LPENUM_VARIANT      lpEnumVarN1,                    // Ptr to ENUM_VARIANT entries
                        lpEnumVarC1;                    // ...

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Check for axis operator on the left operand
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Ensure the right operand is a variable
    if (IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    //***************************************************************
    // Get the attributes (Type, NELM, and Rank) of the right operand
    //    and right arg
    //***************************************************************
    AttrsOfToken (&lpYYFcnStrRht->tkToken, &aplTypeRhtOpr, &aplNELMRhtOpr, &aplRankRhtOpr, NULL);

    // Check for errors
    if (aplNELMRhtOpr > MAX_VAR_LEN)
        goto LIMIT_EXIT;

    if (lptkLftArg NE NULL)
        AttrsOfToken (lptkLftArg             , &aplTypeLft   , &aplNELMLft   , &aplRankLft   , NULL);
    if (lptkRhtArg NE NULL)
        AttrsOfToken (lptkRhtArg             , &aplTypeRht   , &aplNELMRht   , &aplRankRht   , &aplColsRht);

    // If the left operand is neither an immediate function nor function array, or
    //    the right operand is nested, ...
    if ((!IsTknImmed    (&lpYYFcnStrLft->tkToken)
      && !IsTknFcnArray (&lpYYFcnStrLft->tkToken))
      || IsNested (aplTypeRhtOpr))
        return
          PrimOpVariantKeyword_EM_YY (lptkLftArg,               // Ptr to left arg token (may be NULL if monadic derived function)
                                                                //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                                                //     then this value is the original right arg
                                      lpYYFcnStrLft,            // Ptr to left operand function strand
                                      lpPrimFlagsLft,           // Ptr to left operand primitive flags (may be NULL)
                                      lpYYFcnStrRht,            // Ptr to right operand function strand
                                      lptkRhtArg,               // Ptr to right arg token
                                      aplTypeRhtOpr,            // Right operand storage type
                                      aplNELMRhtOpr,            // ...           NELM
                                      aplRankRhtOpr,            // ...           rank
                                      lpMemPTD,                 // Ptr to PerTabData global memory
                                     &lpYYFcnStrOpr->tkToken);  // Ptr to operator oken
    // If the left operand is not immediate and not a function array, ...
    if (!IsTknImmed    (&lpYYFcnStrLft->tkToken)
     && !IsTknFcnArray (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Validate the right operand based upon the left operand
    lpVarOprStr = varOprTab[SymTrans (&lpYYFcnStrLft->tkToken)];
    if (lpVarOprStr EQ NULL)
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Set enumVars ptrs early as they are used at the end
    lpEnumVarN1 = ((lptkLftArg EQ NULL) ? &lpVarOprStr->enumVarMonN1
                                        : &lpVarOprStr->enumVarDydN1);
    lpEnumVarC1 = ((lptkLftArg EQ NULL) ? &lpVarOprStr->enumVarMonC1
                                        : &lpVarOprStr->enumVarDydC1);
    //***************************************************************
    // Ensure that the old value of all sys vars has been saved
    //***************************************************************
    ZeroMemory (&allSysVarsStr, sizeof (allSysVarsStr));
    SaveAllSysVars (&allSysVarsStr ,
                     lpMemPTD      );

    // Mark as initialized
    bInitEnumVars = TRUE;

    // Initialize the maximum length of the right operand
    uMaxLen = 0;

    // Calculate the allowed maximum length of the right operand
    if (lptkLftArg NE NULL)
    {
        // Dyadic case
        // Loop through the numeric ENUMs in lpVarOprStr
        for (uCnt = 0; uCnt < MAX_VAR_NUMLEN; uCnt++)
            // Accumulate the number of valid Variant choices
            uMaxLen += ((&lpVarOprStr->enumVarDydN1)[uCnt] NE ENUM_VARIANT_UNK);

        // Loop through the char ENUMs in lpVarOprStr
        for (uCnt = 0; uCnt < MAX_VAR_CHRLEN; uCnt++)
            // Accumulate the number of valid Variant choices
            uMaxLen += ((&lpVarOprStr->enumVarDydC1)[uCnt] NE ENUM_VARIANT_UNK);
    } else
    {
        // Monadic case
        // Loop through the numeric ENUMs in lpVarOprStr
        for (uCnt = 0; uCnt < MAX_VAR_NUMLEN; uCnt++)
            // Accumulate the number of valid Variant choices
            uMaxLen += ((&lpVarOprStr->enumVarMonN1)[uCnt] NE ENUM_VARIANT_UNK);

        // Loop through the char ENUMs in lpVarOprStr
        for (uCnt = 0; uCnt < MAX_VAR_CHRLEN; uCnt++)
            // Accumulate the number of valid Variant choices
            uMaxLen += ((&lpVarOprStr->enumVarMonC1)[uCnt] NE ENUM_VARIANT_UNK);
    } // End IF/ELSE

    // Validate the right operand as
    //   a simple numeric/character scalar or one- or two-element vector
    if (IsRank2P (aplRankRhtOpr))
        goto RIGHT_OPERAND_RANK_EXIT;
    if (1 > aplNELMRhtOpr
     ||     aplNELMRhtOpr > uMaxLen)
        goto RIGHT_OPERAND_LENGTH_EXIT;
    if (!IsNumeric   (aplTypeRhtOpr)
     && !IsSimpleChar(aplTypeRhtOpr)
     && !IsSimpleHet (aplTypeRhtOpr))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Get right operand's global ptrs
    lpMemRhtOpr = GetGlbMem_LOCK (&lpYYFcnStrRht->tkToken, &hGlbRhtOpr, (LPVOID *) &lpMemHdrRhtOpr, &aplLongestRhtOpr);

    // If it's heterogeneous, ...
    if (IsSimpleHet (aplTypeRhtOpr))
    {
        // Loop through the right operand
        for (uNum = uChr = uRhtOpr = 0; uRhtOpr < aplNELMRhtOpr; uRhtOpr++)
        // Check the next entry
        if (!PrimOpVariantCheckHetero_EM (lpMemRhtOpr,              // Ptr to right operand global memory data
                                          uRhtOpr,                  // Index into lpMemRhtOpr to use
                                         &eFound[uRhtOpr],          // Ptr to eFound array
                                         &uNum,                     // ...    Numeric count so far
                                         &uChr,                     // ...    Char    ...
                                         &aplRhtOprItm[uRhtOpr],    // ...    result  array
                                          varOprType[lpEnumVarN1[uNum]], // Convert to this datatype if numeric
                                         &lpYYFcnStrOpr->tkToken,   // Ptr to operator token
                                         &lpYYFcnStrRht->tkToken))  // Ptr to right operand token
            goto ERROR_EXIT;
    } else
    {
        // Loop through the right operand
        for (uNum = uChr = uRhtOpr = 0; uRhtOpr < aplNELMRhtOpr; uRhtOpr++)
        // Check the next entry
        if (!PrimOpVariantCheckSimple_EM (lpMemRhtOpr,              // Ptr to right operand global memory data
                                          aplTypeRhtOpr,            // Right operand storage type
                                          uRhtOpr,                  // Index into lpMemRhtOpr to use
                                         &eFound[uRhtOpr],          // Ptr to eFound array
                                         &uNum,                     // ...    Numeric count so far
                                         &uChr,                     // ...    Char    ...
                                         &aplRhtOprItm[uRhtOpr],    // ...    result array
                                          varOprType[lpEnumVarN1[uNum]], // Convert to this datatype if numeric
                                         &lpYYFcnStrOpr->tkToken,   // Ptr to operator token
                                         &lpYYFcnStrRht->tkToken))  // Ptr to right operand token
            goto ERROR_EXIT;
    } // End IF/ELSE

    //***************************************************************
    //  Set the new values of all sys vars
    //***************************************************************

    // Loop through the found items
    for (uNum = uChr = uCnt = 0; uCnt < aplNELMRhtOpr; uCnt++)
    // Split cases based upon the eFound state
    switch (eFound[uCnt])
    {
        case ENUM_NCF_NUM:
            if (lpEnumVarN1[uNum] EQ ENUM_VARIANT_UNK)
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Set all sys vars to the appropriate value
            SetAllSysVars (lpEnumVarN1[uNum++] ,
                           aplRhtOprItm[uCnt]  ,
                          &allSysVarsStr       ,
                           lpMemPTD            );
            break;

        case ENUM_NCF_CHR:
            if (lpEnumVarC1[uChr] EQ ENUM_VARIANT_UNK)
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Set all sys vars to the appropriate value
            SetAllSysVars (lpEnumVarC1[uChr++] ,
                           aplRhtOprItm[uCnt]  ,
                          &allSysVarsStr       ,
                           lpMemPTD            );
            break;

        defstop
            break;
    } // End FOR/SWITCH

    // ***TESTME*** -- Handle axis operator on a PSDF

    // Split cases based upon the immediate function or the principal function/operator in the function array
    switch (lpYYFcnStrLft->tkToken.tkData.tkChar)
    {
        // []FPC:  Execute
        case UTF16_UPTACKJOT:
            // Ensure there's no left arg
            if (lptkLftArg NE NULL)
                goto VALENCE_EXIT;

            // Check for axis operator
            lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

            // Get the magic function/operator global memory handle
            hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MonExecute];

            lpYYRes =
              ExecuteMagicFunction_EM_YY (&lpYYFcnStrRht->tkToken,              // Ptr to left arg (really right operand) token
                                          &lpYYFcnStrOpr->tkToken,              // Ptr to function token
                                           lpYYFcnStrOpr,                       // Ptr to function strand
                                           lptkRhtArg,                          // Ptr to right arg token
                                           lptkAxisOpr,                         // Ptr to axis token
                                           hGlbMFO,                             // Magic function/operator global memory handle
                                          &lpMemPTD->ahtsMFO[HTS_MONEXECUTE],   // Ptr to HSHTAB struc (may be NULL)
                                           bPrototyping
                                         ? LINENUM_PRO
                                         : LINENUM_ONE);                        // Starting line # type (see LINE_NUMS)
            break;

        // Pochhammer Symbol (Rising and Falling factorials):
        // See https://en.wikipedia.org/wiki/Falling_and_rising_factorials
        case UTF16_QUOTEDOT:                // Monadic only
            // Ensure there's no left arg
            if (lptkLftArg NE NULL)
                goto VALENCE_EXIT;

            // Get the magic function/operator global memory handle
            hGlbMFO = lpMemPTD->hGlbMFO[MFOE_DydVOFact];

            lpYYRes =
              ExecuteMagicFunction_EM_YY (lptkLftArg,               // Ptr to left arg token
                                         &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                          lpYYFcnStrOpr,            // Ptr to function strand
                                          lptkRhtArg,               // Ptr to right arg token
                                         &lpYYFcnStrRht->tkToken,   // Ptr to axis token (from right operand)
                                          hGlbMFO,                  // Magic function/operator global memory handle
                                          NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                          bPrototyping
                                        ? LINENUM_PRO
                                        : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
            break;

        // Ascending/Descending subsequence
        case UTF16_IOTA:
            // If we're calculating Ascending/Descending Subsequence stuff, ...
            if (lptkLftArg NE NULL
             && allSysVarsStr.cSubNew NE L'\0')
            {
                lpYYRes =
                  PrimOpVariantSubseq_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             aplTypeLft,                    // Left arg storage type
                                             aplRankLft,                    // ...      rank
                                             aplNELMLft,                    // ...      NELM
                                             lptkRhtArg,                    // Ptr to right arg token
                                             aplTypeRht,                    // Right arg storage type
                                             aplRankRht,                    // ...       rank
                                             aplNELMRht,                    // ...       NELM
                                             allSysVarsStr.cSubNew,         // Right operand char value
                                             lpYYFcnStrOpr);                // Ptr to operator function strand
                break;
            } // End IF

            goto DEFAULT;

        // Eigenvalues/Eigenvectors/Schur Vectors
        case UTF16_DOMINO:
            // If we're calculating Eigen stuff, ...
            if (allSysVarsStr.uEigenNew NE 0)
            {
                // Calculate Eigen stuff
                lpYYRes =
                  PrimOpVariantDomino_EM_YY (lptkRhtArg,                // Ptr to right arg token
                                             allSysVarsStr.uEigenNew,   // Right operand numeric value
                                             aplTypeRht,                // Right arg storage type
                                             aplRankRht,                // Right arg rank
                                             aplColsRht,                // Right arg # cols
                                             aplNELMRht,                // Right arg NELM
                                             lpYYFcnStrOpr);            // Ptr to operator function strand
                break;
            } // End IF

            goto DEFAULT;

        // Grade All
        case UTF16_DELSTILE:
        case UTF16_DELTASTILE:
            // If we're called monadically, ...
            if (lptkLftArg EQ NULL)
            {
                // Calculate Grade All
                lpYYRes =
                  PrimFnMonGradeCommon_EM_YY (&lpYYFcnStrLft->tkToken,  // Ptr to function token
                                               lptkRhtArg,              // Ptr to right arg token
                                               lptkAxisLft,             // Ptr to axis token (may be NULL)
                                               FALSE,                   // TRUE iff we're to treat the right arg as ravelled
                                               TRUE);                   // TRUE iff we can grade all arrays
                break;
            } // End IF

            goto DEFAULT;

        default:
        DEFAULT:
            // If this is Inner Product identity function, ...
            if (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_DOT
             && lpPrimFlagsLft NE NULL)
                // Execute the left operand identity function on the right arg
                lpYYRes =
                  (*lpPrimFlagsLft->lpPrimOps)
                                    (lptkLftArg,        // Ptr to original right arg token
                                     lpYYFcnStrLft,     // Ptr to function strand
                                     lptkRhtArg,        // Ptr to right arg token
                                     NULL);             // Ptr to axis token (may be NULL)
            else
                // Execute the function
                lpYYRes =
                  ExecFunc_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                                  lpYYFcnStrLft,        // Ptr to function strand
                                  lptkRhtArg);          // Ptr to right arg token
            break;
    } // End SWITCH

    // Check for error
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;
    else
        goto NORMAL_EXIT;

    goto NORMAL_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

LIMIT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (lpYYRes NE NULL)
    {
        // Free the YYRes
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF
NORMAL_EXIT:
    // If the <lpEnumVarXX>s have been initialized, ...
    if (bInitEnumVars)
    // Loop through the found items
    for (uNum = uChr = uCnt = 0; uCnt < aplNELMRhtOpr; uCnt++)
    // Split cases based upon the eFound state
    switch (eFound[uCnt])
    {
        case ENUM_NCF_NUM:
            // Restore all sys vars
            RestAllSysVars (lpEnumVarN1[uNum++] ,
                           &allSysVarsStr       ,
                            lpMemPTD            );
            break;

        case ENUM_NCF_CHR:
            // Restore all sys vars
            RestAllSysVars (lpEnumVarC1[uChr++] ,
                           &allSysVarsStr       ,
                            lpMemPTD            );
            break;

////////default:
    } // End IF/FOR/SWITCH

    if (hGlbRhtOpr NE NULL
     && lpMemHdrRhtOpr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRhtOpr); lpMemHdrRhtOpr = NULL;
    } // End IF

    return lpYYRes;
} // End PrimOpVariantCom_EM_YY


//***************************************************************************
//  Magic functions/operators for Variant Operator
//***************************************************************************

#include "mf_variant.h"

/// //***************************************************************************
/// //  Magic function/operator for circular function divisor from the Variant Operator
/// //***************************************************************************
///
/// static APLCHAR DydVOCircHeader[] =
///   L"Z" $IS L"L " MFON_DydVOCirc L"[X] R;" $QUAD_IO;
///
/// static APLCHAR DydVOCircLine1[] =
///   L"Z" $IS L"L" $CIRCLE $CIRCLE L"R" $DIVIDE L"2" $TIMES L"X";
///
/// static LPAPLCHAR DydVOCircBody[] =
/// {DydVOCircLine1,
/// };
///
/// MAGIC_FCNOPR MFO_DydVOCirc =
/// {DydVOCircHeader,
///  DydVOCircBody,
///  countof (DydVOCircBody),
/// };


//***************************************************************************
//  $PrimOpVariantSubseq_EM_YY
//
//  Variant applied to iota to find Ascending/Descending subsequences.
//***************************************************************************

LPPL_YYSTYPE PrimOpVariantSubseq_EM_YY
    (LPTOKEN           lptkLftArg,                      // Ptr to left arg token
     APLSTYPE          aplTypeLft,                      // Left arg storage type
     APLRANK           aplRankLft,                      // ...      rank
     APLNELM           aplNELMLft,                      // ...      NELM
     LPTOKEN           lptkRhtArg,                      // Ptr to right arg token
     APLSTYPE          aplTypeRht,                      // Right arg storage type
     APLRANK           aplRankRht,                      // ...       rank
     APLNELM           aplNELMRht,                      // ...       NELM
     APLCHAR           aplChrRhtOpr,                    // Right operand char value
     LPPL_YYSTYPE      lpYYFcnStrOpr)                   // Ptr to operator function strand

{
    LPPL_YYSTYPE        lpYYRes = NULL;                 // Ptr to the result
    APLSTYPE            aplTypeCom;                     // Common storage type
    HGLOBAL             hGlbLft = NULL,                 // Left arg global memory handle
                        hGlbRht = NULL,                 // Right ...
                        hGlbRes = NULL;                 // Result   ...
    LPVARARRAY_HEADER   lpMemHdrLft = NULL,             // Ptr to left arg global memory header
                        lpMemHdrRht = NULL,             // ...    right  ...
                        lpMemHdrRes = NULL;             // ...    result ...
    APLLONGEST          aplLongestLft,                  // Left arg if immediate
                        aplLongestRht;                  // Right ...
    LPVOID              lpMemLft,                       // Ptr to left arg global memory
                        lpMemRht,                       // ...    right ...
                        lpMemRes;                       // ...    result ...
    UBOOL               bQuadIO = GetQuadIO (),         // []IO
                        bRet;                           // TRUE iff the result of hcXY_cmp is equal
    APLFLOAT            fQuadCT = GetQuadCT ();         // []CT
    APLINT              iLft,                           // Loop counter
                        iRht,                           // ...
                        iOff,                           // Offset
                        iNotFound,                      // Not Found value
                        iHighInL;                       // Highest index into L
    ALLTYPES            atLft = {0},                    // Left arg as ALLTYPES
                        atRht = {0};                    // Right ...

    // Get left & right arg's global ptrs
    lpMemLft = GetGlbMem_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft, &aplLongestLft);
    lpMemRht = GetGlbMem_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht, &aplLongestRht);

    // Check for RANK ERRORs
    if (!IsVector (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for RANK ERRORs
    if (!IsVector (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Allocate a global array for the result
    hGlbRes = AllocateGlobalArray (ARRAY_INT, aplNELMRht, 1, &aplNELMRht);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Calculate the common storage type
    aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

    // Check for errors
    if (IsErrorType (aplTypeCom))
        goto DOMAIN_EXIT;

    // Split cases based upon the Ascending/Descending flag
    switch (aplChrRhtOpr)
    {
        case L'a':
            // Initialize vars
            iOff      = 0;
            iNotFound = aplNELMLft + bQuadIO;

            // Loop through the right arg
            for (iRht = 0; iRht < (APLINT) aplNELMRht; iRht++)
            {
                // Promote the next item from the right arg to the common type
                (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemRht, iRht, &atRht);

                // Loop through the left arg
                for (iLft = iOff; iLft < (APLINT) aplNELMLft; iLft++)
                {
                    // Promote the next item from the left arg to the common type
                    (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpMemLft, iLft, &atLft);

                    // Compare the two items
                    bRet =
                      hcXY_cmp (aplTypeCom,
                               &atLft,
                               &atRht,
                                TRUE,
                                fQuadCT) EQ 0;
                    // Free the old atLft (if any)
                    (*aTypeFree[aplTypeCom]) (&atLft, 0);

                    // If they are equal, ...
                    if (bRet)
                    {
                        // Start in the left arg at the next item
                        iOff = iLft + 1;

                        // Save this index in the result
                        ((LPAPLINT) lpMemRes)[iRht] = iLft + bQuadIO;

                        break;
                    } // End IF
                } // End FOR

                // If we reached the end of the left arg, ...
                if (iLft EQ aplNELMLft)
                {
                    // Save the NotFound value in the result
                    ((LPAPLINT) lpMemRes)[iRht] = iNotFound;

                    // Set the next offset
                    iOff = aplNELMLft;
                } // End IF

                // Free the old atRht (if any)
                (*aTypeFree[aplTypeCom]) (&atRht, 0);
            } // End FOR

            break;

        case L'd':
            // Initialize vars
            iOff        = 0;
            iNotFound   = ((APLINT) bQuadIO) - 1;   // Need cast because bQuadIO returns an unsigned value
            iHighInL    = aplNELMLft - 1;

            // Loop through the right arg
            for (iRht = 0; iRht < (APLINT) aplNELMRht; iRht++)
            {
                // Promote the next item from the right arg to the common type
                (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemRht, iRht, &atRht);

                // Loop through the left arg
                for (iLft = iHighInL - iOff; iLft >= 0; iLft--)
                {
                    // Promote the next item from the left arg to the common type
                    (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpMemLft, iLft, &atLft);

                    // Compare the two items
                    bRet =
                      hcXY_cmp (aplTypeCom,
                               &atLft,
                               &atRht,
                                TRUE,
                                fQuadCT) EQ 0;

                    // Free the old atLft (if any)
                    (*aTypeFree[aplTypeCom]) (&atLft, 0);

                    // If they are equal, ...
                    if (bRet)
                    {
                        // Start in the left arg at the next item
                        iOff = aplNELMLft - iLft;

                        // Save this index in the result
                        ((LPAPLINT) lpMemRes)[iRht] = iLft + bQuadIO;

                        break;
                    } // End IF
                } // End FOR

                // If we reached the end of the left arg, ...
                if (iLft < 0)
                {
                    // Save the NotFound value in the result
                    ((LPAPLINT) lpMemRes)[iRht] = iNotFound;

                    // Set the next offset
                    iOff = aplNELMLft;
                } // End IF

                // Free the old atRht (if any)
                (*aTypeFree[aplTypeCom]) (&atRht, 0);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        MyGlobalFree (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemHdrLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemHdrRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemHdrRes); lpMemHdrRes = NULL;
    } // End IF

    return lpYYRes;
} // End PrimOpVariantSubseq_EM_YY


//***************************************************************************
//  $PrimOpVariantDomino_EM_YY
//
//  Variant applied to domino to produce Eigenvalues, Eigenvectors, etc.
//***************************************************************************

LPPL_YYSTYPE PrimOpVariantDomino_EM_YY
    (LPTOKEN           lptkRhtArg,                      // Ptr to right arg token
     APLLONGEST        aplLngRhtOpr,                    // Right operand numeric value
     APLSTYPE          aplTypeRht,                      // Right arg storage type
     APLRANK           aplRankRht,                      // Right arg rank
     APLNELM           aplColsRht,                      // Right arg # cols
     APLNELM           aplNELMRht,                      // Right arg NELM
     LPPL_YYSTYPE      lpYYFcnStrOpr)                   // Ptr to operator function strand

{
    HGLOBAL             hGlbRht = NULL;                 // Right arg global memory handle
    LPVARARRAY_HEADER   lpMemHdrRht = NULL;             // Ptr to right arg global memory header
    APLLONGEST          aplLongestRht;                  // Right arg if immediate
    LPPL_YYSTYPE        lpYYRes = NULL;                 // Ptr to the result
    UBOOL               bRet = TRUE;                    // TRUE iff the result is valid
    APLDIM              aplRowsRht;                     // Right arg # rows
    int                 i,                              // Loop counter
                        ErrCode;                        // Error code
    APLDIM              NxN[2],                         // The array dimensions
                        iTwo = 2,                       // Constant 2
                        iThree = 3;                     // ...      3
    HGLOBAL             hGlbRes = NULL,                 // Result        ...
                        hGlbRht2 = NULL,                // Base arg global memory handle
                        hGlbEval = NULL,                // Eigenvalues   ...
                        hGlbEvec = NULL,                // Eigenvectors  ...
                        hGlbSchur = NULL,               // Schur vectors ...
                        hGlbQ = NULL,                   // Q matrix
                        hGlbR = NULL;                   // R ...
    LPVARARRAY_HEADER   lpMemHdrRes = NULL,             // ...    result    ...
                        lpMemHdrEval = NULL,            // ...    Evalues   ...
                        lpMemHdrEvec = NULL,            // ...    Evectors  ...
                        lpMemHdrSchur = NULL,           // ...    Schur vectors  ...
                        lpMemHdrQ = NULL,               // ...    Q matrix  ...
                        lpMemHdrR = NULL;               // ...    R ...     ...
    LPVOID              lpMemRes,                       // Ptr to result
                        lpMemRht;                       // ...    right arg
    LPAPLFLOAT          lpMemEval,                      // Ptr to global memory data
                        lpMemEvec,                      // ...
                        lpMemSchur,                     // ...
                        lpMemQ,                         // ...
                        lpMemR;                         // ...
    gsl_matrix         *lpGslMatrixA = NULL,            // GSL Temp
                       *lpGslMatrixQ = NULL,            // ...
                       *lpGslMatrixR = NULL,            // ...
                       *lpGslMatrixZ = NULL;            // ...
    gsl_vector         *lpGslVectorTau = NULL;          // ...
    gsl_vector_complex *lpGslCVectorEval = NULL;        // Eigenvalues
    gsl_matrix_complex *lpGslCMatrixEvec = NULL;        // Eigenvectors
    gsl_eigen_nonsymmv_workspace *lpGslEigenWs = NULL;  // Ptr to the GSL workspace

    // Get right arg's global ptrs
    lpMemRht = GetGlbMem_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht, &aplLongestRht);

    // Validate the right arg as a square matrix
    if (!IsMatrix (aplRankRht))
        goto RIGHT_RANK_EXIT;

    if (IsZeroDim (aplColsRht))
        aplRowsRht = 0;
    else
        aplRowsRht = aplNELMRht / aplColsRht;

    // If it's not square, ...
    if (aplColsRht NE aplRowsRht)
        goto RIGHT_LENGTH_EXIT;

    // Save as the two dimensions of the matrix
    NxN[0] =
    NxN[1] = aplColsRht;

    // Check for DOMAIN ERROR or type demotion
    if (!(IsRealBIA (aplTypeRht)
       || IsRealFlt (aplTypeRht)))
    {
        APLSTYPE aplTypeRht2;           // Right arg base storage type

        // Calculate the right arg base storage type
        aplTypeRht2 = aToSimple[aplTypeRht];

        // If the demoted type is not a simple integer or float, ...
        if (!(IsRealBIA (aplTypeRht2)
           || IsRealFlt (aplTypeRht2)))
            goto RIGHT_DOMAIN_EXIT;

        // Allocate new and Demote the right arg
        hGlbRht2 = AllocateDemote (aplTypeRht2,         // Base storage type
                                   hGlbRht,             // Right arg global memory handle (may be NULL
                                   NULL,                // Ptr to ALLTYPES values (may be NULL)
                                   aplTypeRht,          // ... storage type
                                   aplNELMRht,          // ... NELM
                                   aplRankRht,          // ... rank
                                  &bRet);               // TRUE iff the result is not a WS FULL
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

        // Check for error
        if (!bRet)
            goto RIGHT_DOMAIN_EXIT;
        if (hGlbRht2 EQ NULL)
            goto WSFULL_EXIT;
        // Save as the new global handle and storage type
        hGlbRht    = hGlbRht2;
        aplTypeRht = aplTypeRht2;

        // Lock the memory to get a ptr to it
        lpMemHdrRht = MyGlobalLockVar (hGlbRht);

        // Point to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

        // Note that we need to delete the right arg (hGlbRht2) on exit
    } // End IF

    // Sadly, GSL ABORTS! if asked to handle an empty array:  Boo
    if (!IsZeroDim (aplColsRht))
    {
        // If we're calculating the QR matrices, ...
        if (aplLngRhtOpr EQ 5)
        {
            // Allocate GSL arrays for case 5
            lpGslMatrixA   = gsl_matrix_alloc ((int) aplRowsRht, (int) aplColsRht);     // N x N
            lpGslMatrixQ   = gsl_matrix_alloc ((int) aplRowsRht, (int) aplColsRht);     // N x N
            lpGslMatrixR   = gsl_matrix_alloc ((int) aplRowsRht, (int) aplColsRht);     // N x N
            lpGslVectorTau = gsl_vector_alloc (                  (int) aplColsRht);     // N

            // Check the return codes for the above allocations
            if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixA
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixQ
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixR
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslVectorTau
               )
                goto WSFULL_EXIT;
        } else
        {
            // Allocate GSL arrays for cases 1-4
            lpGslMatrixA     = gsl_matrix_alloc         ((int) aplRowsRht, (int) aplColsRht);   // N x N
            lpGslCVectorEval = gsl_vector_complex_alloc (                  (int) aplColsRht);   // N
            lpGslCMatrixEvec = gsl_matrix_complex_alloc ((int) aplRowsRht, (int) aplColsRht);   // N x N
            lpGslEigenWs     = gsl_eigen_nonsymmv_alloc (                  (int) aplColsRht);   // N
            lpGslMatrixZ     = gsl_matrix_alloc         ((int) aplRowsRht, (int) aplColsRht);   // N x N

            // Check the return codes for the above allocations
            if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixA
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslCVectorEval
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslCMatrixEvec
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslEigenWs
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixZ
               )
                goto WSFULL_EXIT;
        } // End IF/ELSE

        // Loop through all of the elements
        for (i = 0; i < aplNELMRht; i++)
            // Copy the next element from the right arg as a float
            lpGslMatrixA->data[i] = GetNextFloat (lpMemRht, aplTypeRht, i);

        // If we're calculating the QR matrices, ...
        if (aplLngRhtOpr EQ 5)
        {
            ErrCode =
              gsl_linalg_QR_decomp (lpGslMatrixA,
                                    lpGslVectorTau);
            // Check the error code
            if (ErrCode NE GSL_SUCCESS)
                goto RIGHT_DOMAIN_EXIT;
            // Unpack the QR matrices
            ErrCode =
              gsl_linalg_QR_unpack (lpGslMatrixA,
                                    lpGslVectorTau,
                                    lpGslMatrixQ,
                                    lpGslMatrixR);
        } else
        // If we're calculating Eigenvalues, Eigenvectors, and Schur vectors, ...
        if (aplLngRhtOpr EQ 4)
            // Calculate the Eigenvalues, Eigenvectors, and Schur vectors
            ErrCode =
              gsl_eigen_nonsymmv_Z (lpGslMatrixA,
                                    lpGslCVectorEval,
                                    lpGslCMatrixEvec,
                                    lpGslMatrixZ,
                                    lpGslEigenWs);
        else
            // Calculate the Eigenvalues and Eigenvectors
            ErrCode =
              gsl_eigen_nonsymmv   (lpGslMatrixA,
                                    lpGslCVectorEval,
                                    lpGslCMatrixEvec,
                                    lpGslEigenWs);
        // Check the error code
        if (ErrCode NE GSL_SUCCESS)
            goto RIGHT_DOMAIN_EXIT;
    } // End IF

    // Split cases based upon the right operand value
    switch (aplLngRhtOpr)
    {
        case 1:     // DEF_MIN_QUADEIG
            // Allocate a global array for the result
            hGlbRes = AllocateGlobalArray (ARRAY_HC2F, aplColsRht, 1, &NxN[0]);

            break;

        case 2:
            // Allocate a global array for the result
            hGlbRes = AllocateGlobalArray (ARRAY_HC2F, aplNELMRht, 2, &NxN[0]);

            break;

        case 3:
            // Allocate a global array for the result
            hGlbRes = AllocateGlobalArray (ARRAY_NESTED, iTwo, 1, &iTwo);

            break;

        case 4:
            // Allocate a global array for the result
            hGlbRes = AllocateGlobalArray (ARRAY_NESTED, iThree, 1, &iThree);

            break;

        case 5:     // DEF_MAX_QUADEIG
            // Allocate a global array for the result
            hGlbRes = AllocateGlobalArray (ARRAY_NESTED, iTwo, 1, &iTwo);

            break;

        defstop
            break;
    } // End SWITCH

    // Check for error
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Split cases based upon the right operand value
    switch (aplLngRhtOpr)
    {
        case 1:     // DEF_MIN_QUADEIG
            // Copy the Eigenvalues to the result
            CopyMemory (lpMemRes, lpGslCVectorEval->data, (APLU3264) (aplColsRht * 2 * sizeof (APLFLOAT)));

            break;

        case 2:
            // Copy the Eigenvectors to the result
            CopyMemory (lpMemRes, lpGslCMatrixEvec->data, (APLU3264) (aplNELMRht * 2 * sizeof (APLFLOAT)));

            // Convert tiny FLTs to zero
            ConvertTinyFlt2Zero (hGlbRes);

            break;

        case 3:
        case 4:
            // Allocate a global array for a vector of Complex Eigenvalues
            hGlbEval  = AllocateGlobalArray (ARRAY_HC2F, aplColsRht, 1, &NxN[0]);

            // Allocate a global array for a matrix of Complex Eigenvectors
            hGlbEvec  = AllocateGlobalArray (ARRAY_HC2F, aplNELMRht, 2, &NxN[0]);

            // If we're also calculating Schur vectors, ...
            if (aplLngRhtOpr EQ 4)
            {
                // Allocate a global array for a matrix of Real Schur vectors
                hGlbSchur = AllocateGlobalArray (ARRAY_HC1F, aplNELMRht, 2, &NxN[0]);

                // Check for errors
                if (hGlbSchur EQ NULL)
                    goto WSFULL_EXIT;
            } // End IF

            // Check for errors
            if (hGlbEval EQ NULL
             || hGlbEvec EQ NULL)
                goto WSFULL_EXIT;

            // If there are any cols, ...
            if (!IsZeroDim (aplColsRht))
            {
                //***************************************************************
                //  Copy Eigenvalues to an item in the result
                //***************************************************************

                // Lock the memory to get a ptr to it
                lpMemHdrEval = MyGlobalLockVar (hGlbEval);

                // Skip over the header and dimensions to the data
                lpMemEval = VarArrayDataFmBase (lpMemHdrEval);

                // Copy the Eigenvalues to the result
                CopyMemory (lpMemEval, lpGslCVectorEval->data, (APLU3264) (aplColsRht * 2 * sizeof (APLFLOAT)));

                //***************************************************************
                //  Copy Eigenvectors to an item in the result
                //***************************************************************

                // Lock the memory to get a ptr to it
                lpMemHdrEvec = MyGlobalLockVar (hGlbEvec);

                // Skip over the header and dimensions to the data
                lpMemEvec = VarArrayDataFmBase (lpMemHdrEvec);

                // Copy the Eigenvectors to the result
                CopyMemory (lpMemEvec, lpGslCMatrixEvec->data, (APLU3264) (aplNELMRht * 2 * sizeof (APLFLOAT)));

                // Convert tiny FLTs to zero
                ConvertTinyFlt2Zero (hGlbEvec);

                // If we're also calculating Schur vectors, ...
                if (aplLngRhtOpr EQ 4)
                {
                    //***************************************************************
                    //  Copy Schur vectors to an item in the result
                    //***************************************************************

                    // Lock the memory to get a ptr to it
                    lpMemHdrSchur = MyGlobalLockVar (hGlbSchur);

                    // Skip over the header and dimensions to the data
                    lpMemSchur = VarArrayDataFmBase (lpMemHdrSchur);

                    // Copy the Schur vectors to the result
                    CopyMemory (lpMemSchur, lpGslMatrixZ->data    , (APLU3264) (aplNELMRht     * sizeof (APLFLOAT)));

                    // Convert tiny FLTs to zero
                    ConvertTinyFlt2Zero (hGlbSchur);

                    // Unlock the result global memory in case TypeDemote actually demotes
                    MyGlobalUnlock (hGlbSchur); lpMemHdrSchur = NULL;

                    // See if it fits into a lower (but not necessarily smaller) datatype
                    //   especially as there are likely to be Complex numbers with zero imaginary parts
                    //   from the Eigen value/vector calculations
                    hGlbSchur = TypeDemoteGlb (hGlbSchur, TRUE);
                } // End IF

                // Unlock the result global memory in case TypeDemote actually demotes
                MyGlobalUnlock (hGlbEval); lpMemHdrEval = NULL;
                MyGlobalUnlock (hGlbEvec); lpMemHdrEvec = NULL;

                // See if it fits into a lower (but not necessarily smaller) datatype
                //   especially as there are likely to be Complex numbers with zero imaginary parts
                //   from the Eigen value/vector calculations
                hGlbEval = TypeDemoteGlb (hGlbEval, TRUE);
                hGlbEvec = TypeDemoteGlb (hGlbEvec, TRUE);
            } // End IF

            // Save the global memory handles in the result
            ((LPAPLNESTED) lpMemRes)[0] = MakePtrTypeGlb (hGlbEval);
            ((LPAPLNESTED) lpMemRes)[1] = MakePtrTypeGlb (hGlbEvec);

            // If we're also calculating Schur vectors, ...
            if (aplLngRhtOpr EQ 4)
                ((LPAPLNESTED) lpMemRes)[2] = MakePtrTypeGlb (hGlbSchur);
            break;

        case 5:     // DEF_MAX_QUADEIG
            // Allocate a global array for the Q matrix
            hGlbQ = AllocateGlobalArray (ARRAY_FLOAT, aplNELMRht, 2, &NxN[0]);

            // Allocate a global array for the R matrix
            hGlbR = AllocateGlobalArray (ARRAY_FLOAT, aplNELMRht, 2, &NxN[0]);

            // Check for errors
            if (hGlbQ EQ NULL
             || hGlbR EQ NULL)
                goto WSFULL_EXIT;

            // If there are any cols, ...
            if (!IsZeroDim (aplColsRht))
            {
                //***************************************************************
                //  Copy Q to an item in the result
                //***************************************************************

                // Lock the memory to get a ptr to it
                lpMemHdrQ = MyGlobalLockVar (hGlbQ);

                // Skip over the header and dimensions to the data
                lpMemQ = VarArrayDataFmBase (lpMemHdrQ);

                // Copy the Q matrix to the result
                CopyMemory (lpMemQ, lpGslMatrixQ->data, (APLU3264) (aplNELMRht * sizeof (APLFLOAT)));

                // Convert tiny FLTs to zero
                ConvertTinyFlt2Zero (hGlbQ);

                //***************************************************************
                //  Copy R to an item in the result
                //***************************************************************

                // Lock the memory to get a ptr to it
                lpMemHdrR = MyGlobalLockVar (hGlbR);

                // Skip over the header and dimensions to the data
                lpMemR = VarArrayDataFmBase (lpMemHdrR);

                // Copy the R matrix to the result
                CopyMemory (lpMemR, lpGslMatrixR->data, (APLU3264) (aplNELMRht * sizeof (APLFLOAT)));

                // Convert tiny FLTs to zero
                ConvertTinyFlt2Zero (hGlbR);

                // Unlock the result global memory in case TypeDemote actually demotes
                MyGlobalUnlock (hGlbQ); lpMemHdrQ = NULL;
                MyGlobalUnlock (hGlbR); lpMemHdrR = NULL;

                // See if it fits into a lower (but not necessarily smaller) datatype
                //   especially as there are likely to be Complex numbers with zero imaginary parts
                //   from the Eigen value/vector calculations
                hGlbQ =TypeDemoteGlb (hGlbQ, TRUE);
                hGlbR =TypeDemoteGlb (hGlbR, TRUE);
            } // End IF

            // Save the global memory handles in the result
            ((LPAPLNESTED) lpMemRes)[0] = MakePtrTypeGlb (hGlbQ);
            ((LPAPLNESTED) lpMemRes)[1] = MakePtrTypeGlb (hGlbR);

            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    goto NORMAL_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF

    if (hGlbEval NE NULL)
    {
        if (lpMemHdrEval NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbEval); lpMemHdrEval = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbEval); hGlbEval = NULL;
    } // End IF

    if (hGlbEvec NE NULL)
    {
        if (lpMemHdrEvec NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbEvec); lpMemHdrEvec = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbEvec); hGlbEvec = NULL;
    } // End IF

    if (hGlbSchur NE NULL)
    {
        if (lpMemHdrSchur NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbSchur); lpMemHdrSchur = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbSchur); hGlbSchur = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes NE NULL
     && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbRht2 NE NULL)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbRht2); hGlbRht2 = NULL;
    } // End IF

    if (hGlbR NE NULL
     && lpMemHdrR NE NULL)
    {
         // We no longer need this ptr
        MyGlobalUnlock (hGlbR); lpMemHdrR = NULL;
    } // End IF

    if (hGlbQ NE NULL
     && lpMemHdrQ NE NULL)
    {
         // We no longer need this ptr
        MyGlobalUnlock (hGlbQ); lpMemHdrQ = NULL;
    } // End IF

    if (hGlbEval NE NULL
     && lpMemHdrEval NE NULL)
    {
         // We no longer need this ptr
        MyGlobalUnlock (hGlbEval); lpMemHdrEval = NULL;
    } // End IF

    if (hGlbEvec NE NULL
     && lpMemHdrEvec NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbEvec); lpMemHdrEvec = NULL;
    } // End IF

    if (hGlbSchur NE NULL
     && lpMemHdrSchur NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbSchur); lpMemHdrSchur = NULL;
    } // End IF

    if (lpGslMatrixZ NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixZ); lpGslMatrixZ = NULL;
    } // End IF

    if (lpGslMatrixR NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixR); lpGslMatrixR = NULL;
    } // End IF

    if (lpGslMatrixQ NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixQ); lpGslMatrixQ = NULL;
    } // End IF

    if (lpGslVectorTau NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_free (lpGslVectorTau); lpGslVectorTau = NULL;
    } // End IF

    if (lpGslCMatrixEvec NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_complex_free (lpGslCMatrixEvec); lpGslCMatrixEvec = NULL;
    } // End IF

    if (lpGslCVectorEval NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_complex_free (lpGslCVectorEval); lpGslCVectorEval = NULL;
    } // End IF

    if (lpGslMatrixA NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixA); lpGslMatrixA = NULL;
    } // End IF

    if (hGlbRht NE NULL
     && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimOpVariantDomino_EM_YY


//***************************************************************************
//  $PrimOpVariantCheckSimple_EM
//
//  Check a simple arg for a char or an integer
//***************************************************************************

UBOOL PrimOpVariantCheckSimple_EM
    (LPVOID       lpMemRhtOpr,          // Ptr to right operand global memory data
     APLSTYPE     aplTypeRhtOpr,        // Right operand storage type
     APLINT       uIndex,               // Index into lpMemRhtOpr to use
     LPENUM_NCF   lpeFound,             // Ptr to eFound
     LPAPLUINT    lpuNum,               // ...    Numeric count so far
     LPAPLUINT    lpuChr,               // ...    Char    ...
     LPAPLLONGEST lpaplLongest,         // ...    Num/Chr
     APLSTYPE     aplTypeNum,           // Convert to this datatype if numeric
     LPTOKEN      lptkOpr,              // Ptr to operator token
     LPTOKEN      lptkRhtOpr)           // Ptr to right operand token

{
    UBOOL bRet = TRUE;                  // TRUE iff the result is valid

    // If the type is numeric, ...
    if (IsNumeric (aplTypeRhtOpr))
    {
        ALLTYPES atArg = {0};

        // If it's an error type, ...
        if (IsErrorType (aplTypeNum))
            goto VALENCE_EXIT;

        // Check for too many
        if ((*lpuNum)++ EQ MAX_VAR_NUMLEN)
            goto LIMIT_EXIT;

        // Convert the item's numeric type to the target's numeric type
        (*aTypeActConvert[aplTypeRhtOpr][aplTypeNum]) (lpMemRhtOpr, uIndex, &atArg, &bRet);

        // Check for error
        if (!bRet)
            goto RIGHT_OPERAND_DOMAIN_EXIT;

        // Set the flag
        *lpeFound     = ENUM_NCF_NUM;
        *lpaplLongest = atArg.aplLongest;
    } else
    {
        Assert (IsSimpleChar (aplTypeRhtOpr));

        // Check for too many
        if ((*lpuChr)++ EQ MAX_VAR_CHRLEN)
            goto LIMIT_EXIT;

        // Set the flag
        *lpeFound     = ENUM_NCF_CHR;
        *lpaplLongest = ((LPAPLCHAR) lpMemRhtOpr)[uIndex];
    } // End IF/ELSE

    return TRUE;

VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                               lptkOpr);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtOpr);
    goto ERROR_EXIT;

LIMIT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                               lptkRhtOpr);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End PrimOpVariantCheckSimple_EM


//***************************************************************************
//  $PrimOpVariantCheckHetero_EM
//
//  Check a hetero arg for a char or an integer
//***************************************************************************

UBOOL PrimOpVariantCheckHetero_EM
    (LPAPLNESTED  lpMemRhtOpr,          // Ptr to right operand global memory data
     APLINT       uIndex,               // Index into lpMemRhtOpr to use
     LPENUM_NCF   lpeFound,             // Ptr to eFound
     LPAPLUINT    lpuNum,               // ...    Numeric count so far
     LPAPLUINT    lpuChr,               // ...    Char    ...
     LPAPLLONGEST lpaplLongest,         // ...    Num/Chr
     APLSTYPE     aplTypeNum,           // Convert to this datatype if numeric
     LPTOKEN      lptkOpr,              // Ptr to operator token
     LPTOKEN      lptkRhtOpr)           // Ptr to right operand token

{
    APLSTYPE          aplTypeItm;           // Item storage type
    HGLOBAL           hGlbItm;              // Item global memory handle
    LPVARARRAY_HEADER lpMemHdrItm = NULL;   // Ptr to item's global memory header
    LPVOID            lpSymGlbItm;          // Ptr to item's global memory data
    UBOOL             bRet = TRUE;          // TRUE iff the result is valid

    // Get the item
    hGlbItm = lpMemRhtOpr[uIndex];

    // Split cases based upon the type bits
    switch (GetPtrTypeDir (hGlbItm))
    {
        case PTRTYPE_STCONST:
            // Point to the data
            lpSymGlbItm = &((LPSYMENTRY) hGlbItm)->stData.stLongest;

            // Get the storage type
            aplTypeItm = TranslateImmTypeToArrayType (((LPSYMENTRY) hGlbItm)->stFlags.ImmType);

            break;

        case PTRTYPE_HGLOBAL:
            // Lock the memory to get a ptr to it
            lpMemHdrItm = MyGlobalLockVar (hGlbItm);

            // Point to the data
            lpSymGlbItm = VarArrayDataFmBase (lpMemHdrItm);

            // Get the storage type
            aplTypeItm = lpMemHdrItm->ArrType;

            break;

        defstop
            break;
    } // End SWITCH

    // Call CheckSimple function
    bRet =
      PrimOpVariantCheckSimple_EM (lpSymGlbItm,         // Ptr to right operand global memory data
                                   aplTypeItm,          // Right operand storage type
                                   0,                   // Index into lpMemRhtOpr to use
                                   lpeFound,            // Ptr to eFound
                                   lpuNum,              // ...    Numeric count so far
                                   lpuChr,              // ...    Char    ...
                                   lpaplLongest,        // ...    Num/Chr
                                   aplTypeNum,          // Convert to this datatype if numeric
                                   lptkOpr,             // Ptr to operator token
                                   lptkRhtOpr);         // Ptr to right operand token
    // If we locked global memory, ...
    if (lpMemHdrItm NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbItm); lpMemHdrItm = NULL;
    } // End IF

    return bRet;
} // End PrimOpVariantCheckHetero_EM


//***************************************************************************
//  $PrimOpDydVariant_EM_YY
//
//  Primitive operator for dyadic derived function from Variant ("variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydVariant_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return
      PrimOpDydVariantCom_EM_YY (lptkLftArg,        // Ptr to left arg token
                                 lpYYFcnStrOpr,     // Ptr to operator function strand
                                 lptkRhtArg,        // Ptr to right arg token
                                 FALSE);            // TRUE iff prototyping
} // End PrimOpDydVariant_EM_YY


//***************************************************************************
//  $PrimOpDydVariantCom_EM_YY
//
//  Primitive operator for dyadic derived function from Variant ("variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydVariantCom_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, CheckAxisOper (lpYYFcnStrOpr)               ); Assert (lpYYFcnStrRht NE NULL);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrOpr, CheckAxisOper (lpYYFcnStrOpr), lpYYFcnStrRht); Assert (lpYYFcnStrLft NE NULL);

    return
      PrimOpVariantCom_EM_YY (lptkLftArg,               // Ptr to left arg token (may be NULL if monadic derived function)
                                                        //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                                        //     then this value is the original right arg
                              lpYYFcnStrLft,            // Ptr to left operand function strand
                              NULL,                     // Ptr to left operand primitive flags (may be NULL)
                              lpYYFcnStrOpr,            // Ptr to operator function strand
                              lpYYFcnStrRht,            // Ptr to right operand function strand
                              lptkRhtArg,               // Ptr to right arg token
                              bPrototyping);            // TRUE iff protoyping
} // End PrimOpDydVariantCom_EM_YY


//***************************************************************************
//  $PrimOpVariantKeyword_EM_YY
//
//  Primitive operator variant using keyword right arg
//***************************************************************************

LPPL_YYSTYPE PrimOpVariantKeyword_EM_YY
    (LPTOKEN      lptkLftArg,                           // Ptr to left arg token (may be NULL if monadic derived function)
                                                        //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                                        //     then this value is the original right arg
     LPPL_YYSTYPE lpYYFcnStrLft,                        // Ptr to left operand function strand
     LPPRIMFLAGS  lpPrimFlagsLft,                       // Ptr to left operand primitive flags (may be NULL)
     LPPL_YYSTYPE lpYYFcnStrRht,                        // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg,                           // Ptr to right arg token (may be null if niladic derived function)
     APLSTYPE     aplTypeRhtOpr,                        // Right operand storage type
     APLNELM      aplNELMRhtOpr,                        // ...           NELM
     APLRANK      aplRankRhtOpr,                        // ...           rank
     LPPERTABDATA lpMemPTD,                             // Ptr to PerTabData global memory
     LPTOKEN      lptkOpr)                              // Prt to operator token

{
    LPVARARRAY_HEADER lpMemHdrRhtOpr = NULL;                // Ptr to right operand global memory header
    LPAPLNESTED       lpMemRhtOpr;                          // ...                                data
    HGLOBAL           hGlbRhtOpr = NULL;                    // Right operand global memory handle
    APLINT            iRhtOpr;                              // Loop counter
    VARIANTKEYS       varKey;                               // ...
    LPPL_YYSTYPE      lpYYRes = NULL;                       // Ptr to result
    VARIANTUSESTR     varUseStr[VARIANT_KEY_LENGTH] = {0};  // In use struc
    APLINT            aplIntDepth;                          // Depth of the right operand

    // The right operand is one of the following cases:
    //                              Depth
    //   1.  'XY' n                   2
    //   2.  {enclose} 'XY n          3
    //   3.  ('XY' n) ('ZZ' n) ...    3

    // If the right operand is multirank, ...
    if (IsRank2P (aplRankRhtOpr))
        goto RIGHT_OPERAND_RANK_EXIT;

    // If the right operand is empty, ...
    if (IsEmpty (aplNELMRhtOpr))
        goto NORMAL_EXIT;

    // If the right operand is simple, ...
    if (IsSimple (aplTypeRhtOpr))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Get right operand's global ptrs
    GetGlbPtrs_LOCK (&lpYYFcnStrRht->tkToken, &hGlbRhtOpr, (LPVOID *) &lpMemHdrRhtOpr);

    Assert (hGlbRhtOpr NE NULL);

    // Skip over the header and dimensions to the data
    lpMemRhtOpr = VarArrayDataFmBase (lpMemHdrRhtOpr);

    // Recursively run through the elements of the nested array
    aplIntDepth = PrimFnMonEqualUnderBarGlb_PTB (hGlbRhtOpr);

    // Split cases based upon the right operand depth
    switch (aplIntDepth)
    {
        case 2:
            // If validation of the pair fails, ...
            if (!PrimOpVariantValidPair_EM (lpMemRhtOpr,    // Ptr to right operand global memory
                                            aplTypeRhtOpr,  // Right operand storage type
                                            aplNELMRhtOpr,  // Right operand NELM
                                           &varUseStr[0],   // Ptr to variant use struc
                                            lpMemPTD,       // Ptr to PerTabData global memory
                                            lptkOpr))       // Ptr to operator token
                goto ERROR_EXIT;

            break;

        case 3:
            // Loop through the items backwards
            for (iRhtOpr = aplNELMRhtOpr - 1; iRhtOpr >= 0; iRhtOpr--)
            // If it fails,
            if (!PrimOpVariantValidateGlb_EM (lpMemRhtOpr[iRhtOpr],     // Item global memory handle
                                             &varUseStr[0],             // Ptr to variant use struc
                                              lpMemPTD,                 // Ptr to PerTabData global memory
                                             &lpYYFcnStrRht->tkToken))  // Ptr to function token
                goto ERROR_EXIT;

            break;

        default:
            goto RIGHT_OPERAND_DOMAIN_EXIT;
    } // End SWITCH

    // At this point, we've localised all of the system vars
    //   and changed their current values to what the user specified

    // If we're called as an identity function, ...
    if (lpPrimFlagsLft NE NULL)
        // Execute the left operand identity function on the right arg
        lpYYRes =
          (*lpPrimFlagsLft->lpPrimOps)
                            (lptkLftArg,            // Ptr to original right arg token
                             lpYYFcnStrLft,         // Ptr to function strand
                             lptkRhtArg,            // Ptr to right arg token
                             NULL);                 // Ptr to axis token (may be NULL)
    else
        // Execute the function
        lpYYRes =
          ExecFuncStr_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
                             lpYYFcnStrLft,         // Ptr to function strand
                             lptkRhtArg,            // Ptr to right arg token (may be NULL if niladic)
                             NULL);                 // Ptr to axis token (may be NULL)
    goto NORMAL_EXIT;

RIGHT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbRhtOpr NE NULL && lpMemHdrRhtOpr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRhtOpr); lpMemHdrRhtOpr = NULL;
    } // End IF

    // Restore the old SYMENTRYs
    for (varKey = 0; varKey < countof (varUseStr); varKey++)
    if (varUseStr[varKey].bInuse)
        *lpMemPTD->lphtsPTD->lpSymQuad[aVariantKeyStr[varKey].sysVarIndex] =
          varUseStr[varKey].OldSymEntry;

    return lpYYRes;
} // End PrimOpVariantKeyword_EM_YY


//***************************************************************************
//  $PrimOpVariantValidateGlb_EM
//
//  Validate a single item as either
//      {enclose}'IO' n
// or an element of
//      ('IO' n) ('CT' n) ...
//***************************************************************************

UBOOL PrimOpVariantValidateGlb_EM
    (HGLOBAL         hGlbRhtOpr,        // Right operand global memory handle
     LPVARIANTUSESTR lpVarUseStr,       // Ptr to variant use struc
     LPPERTABDATA    lpMemPTD,          // Ptr to PerTabData global memory
     LPTOKEN         lptkRhtOpr)        // Ptr to right operand token

{
    LPAPLNESTED lpMemRhtOpr = NULL;     // Right operand global memory handle
    UBOOL       bRet = FALSE;           // TRUE iff the result is valid
    APLSTYPE    aplTypeRhtOpr;          // Right operand array storage type
    APLNELM     aplNELMRhtOpr;          // Right operand NELM

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (hGlbRhtOpr))
    {
        case PTRTYPE_STCONST:
            goto RIGHT_OPERAND_DOMAIN_EXIT;

        case PTRTYPE_HGLOBAL:
            // Lock the memory to get a ptr to it
            lpMemRhtOpr = MyGlobalLockVar (hGlbRhtOpr);

            // Get the storage type
            aplTypeRhtOpr = ((LPVARARRAY_HEADER) lpMemRhtOpr)->ArrType;
            aplNELMRhtOpr = ((LPVARARRAY_HEADER) lpMemRhtOpr)->NELM;

            // If it's not a vector, ...
            if (!IsVector (((LPVARARRAY_HEADER) lpMemRhtOpr)->Rank))
                goto RIGHT_OPERAND_RANK_EXIT;

            // If it's not a singleton or pair, ...
            if (aplNELMRhtOpr NE 1
             && aplNELMRhtOpr NE 2)
                goto RIGHT_OPERAND_LENGTH_EXIT;

            // If it's not nested, ...
            if (!IsNested (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Skip over the header and dimensions to the data
            lpMemRhtOpr = VarArrayDataFmBase (lpMemRhtOpr);

            break;

        defstop
            break;
    } // End SWITCH

    // If validation of the pair fails, ...
    if (!PrimOpVariantValidPair_EM (lpMemRhtOpr,    // Ptr to right operand global memory
                                    aplTypeRhtOpr,  // Right operand storage type
                                    aplNELMRhtOpr,  // Right operand NELM
                                    lpVarUseStr,    // Ptr to variant use struc
                                    lpMemPTD,       // Ptr to PerTabData global memory
                                    lptkRhtOpr))    // Ptr to right operand token
        goto ERROR_EXIT;
    else
        goto NORMAL_EXIT;

RIGHT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtOpr);
    goto ERROR_EXIT;

RIGHT_OPERAND_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtOpr);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtOpr);
    goto ERROR_EXIT;

NORMAL_EXIT:
    // Mark as valid
    bRet = TRUE;
ERROR_EXIT:
    if (lpMemRhtOpr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRhtOpr); lpMemRhtOpr = NULL;
    } // End IF

    return bRet;
} // End PrimOpVariantValidateGlb_EM


//***************************************************************************
//  $PrimOpVariantValidPair_EM
//
//  Validate a 'XY' N Keyword/Value pair
//***************************************************************************

UBOOL PrimOpVariantValidPair_EM
    (LPAPLNESTED     lpMemRhtOpr,       // Right operand global memory handle
     APLSTYPE        aplTypeRhtOpr,     // Right operand array storage type
     APLNELM         aplNELMRhtOpr,     // Right operand NELM
     LPVARIANTUSESTR lpVarUseStr,       // Ptr to variant use struc
     LPPERTABDATA    lpMemPTD,          // Ptr to PerTabData global memory
     LPTOKEN         lptkRhtOpr)        // Ptr to right operand token

{
    HGLOBAL     hGlbItm;                // Item global memory handle
    APLLONGEST  aplLongestItm;          // Item immediate value
    IMM_TYPES   immTypeItm;             // Item immediate type
    UBOOL       bRet = FALSE,           // TRUE iff the result is valid
                bReset;                 // TRUE iff the second item is empty
    VARIANTKEYS varKey;                 // Variant key index

    // Get the first value from the right operand
    GetNextValueMem (lpMemRhtOpr,       // Ptr to right operand global memory
                     aplTypeRhtOpr,     // Right operand storage type
                     aplNELMRhtOpr,     // Right operand NELM
                     0,                 // Index to use
                    &hGlbItm,           // Ptr to the LPSYMENTRY or HGLOBAL (may be NULL)
                    &aplLongestItm,     // ...        immediate value (may be NULL)
                    &immTypeItm);       // ...        immediate type:  IMM_TYPES (may be NULL)
    // Check for char
    //   and pair
    if (!IsImmChr (immTypeItm)
     || aplNELMRhtOpr NE 2)
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Validate the keyword
    varKey = PrimOpVariantValKeyGlb (lpMemRhtOpr[0]);
    if (varKey EQ VARIANT_KEY_ERROR)
        goto ERROR_EXIT;

    // Get the second value from the right operand
    GetNextValueMem (lpMemRhtOpr,               // Ptr to right operand global memory
                     aplTypeRhtOpr,             // Right operand storage type
                     aplNELMRhtOpr,             // Right operand NELM
                     1,                         // Index to use
                    &hGlbItm,                   // Ptr to the LPSYMENTRY or HGLOBAL (may be NULL)
                    &aplLongestItm,             // ...        immediate value (may be NULL)
                    &immTypeItm);               // ...        immediate type:  IMM_TYPES (may be NULL)
    // Check for error
    if (IsImmErr (immTypeItm))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Set the reset flag
    if (hGlbItm EQ NULL             // Immediate value
     || IsImmGlbNum (immTypeItm))   // Simple global numeric
        bReset = FALSE;
    else
    {
        APLNELM aplNELMItm;

        AttrsOfGlb (hGlbItm, NULL, &aplNELMItm, NULL, NULL);

        bReset = (aplNELMItm EQ 0);
    } // End IF/ELSE

    // Save the SymEntry, validate the value
    if (!VariantValidateSymVal_EM (immTypeItm,          // Item immediate type
                                   hGlbItm,             // Item global memory handle
                                   aplLongestItm,       // Immediate value
                                   bReset,              // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                   lpVarUseStr,         // Ptr to variant use struc
                                   varKey,              // Variant key index
                                   lpMemPTD,            // Ptr to PerTabData global memory
                                   lptkRhtOpr))         // Ptr to right operand token
        goto ERROR_EXIT;
    else
        goto NORMAL_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtOpr);
    goto ERROR_EXIT;

NORMAL_EXIT:
    // Mark as valid
    bRet = TRUE;
ERROR_EXIT:
    return bRet;
} // End PrimOpVariantValidPair_EM


//***************************************************************************
//  $VariantValidateSymVal
//
//  Save the SymEntry, validate the value
//***************************************************************************

UBOOL VariantValidateSymVal_EM
    (IMM_TYPES       immTypeItm,        // Item immediate type
     HGLOBAL         hGlbItm,           // Item global memory handle
     APLLONGEST      aplLongestItm,     // Item immediate value
     UBOOL           bReset,            // TRUE iff the assignment value is empty (we're resetting to CLEAR WS/System)
     LPVARIANTUSESTR lpVarUseStr,       // Ptr to variant use struc
     VARIANTKEYS     varKey,            // Variant key index
     LPPERTABDATA    lpMemPTD,          // Ptr to PerTabData global memory
     LPTOKEN         lptkRhtOpr)        // Ptr to right operand token

{
    LPSYMENTRY  lpSymEntry;             // Ptr to sysvar SYMENTRY

    // Get a ptr to the SYMENTRY
    lpSymEntry = lpMemPTD->lphtsPTD->lpSymQuad[aVariantKeyStr[varKey].sysVarIndex];

    // If this variant property isn't in use as yet, ...
    if (!lpVarUseStr[varKey].bInuse)
    {
        // Mark as in use
        lpVarUseStr[varKey].bInuse = TRUE;

        // Back up the corresponding SYMENTRY to restore after executing the Variant Operator
        lpVarUseStr[varKey].OldSymEntry = *lpSymEntry;
    } // End IF

    // Validate the value
    return VariantValidateCom_EM (immTypeItm,                               // Immediate type
                                  hGlbItm,                                  // Global memory handle
                                 &aplLongestItm,                            // Ptr to immediate value (ignored if bReset)
                                  bReset,                                   // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                  aVariantKeyStr[varKey].aSysVarValidSet,   // Ptr to validate set function
                                  lpSymEntry,                               // Ptr to sysvar SYMENTRY
                                  lptkRhtOpr);                              // Ptr to right operand token
} // End VariantValidateSymVal_EM


//***************************************************************************
//  $PrimOpVariantValKeyGlb
//
//  Validate a keyword in a global memory handle
//***************************************************************************

VARIANTKEYS PrimOpVariantValKeyGlb
    (HGLOBAL hGlbKey)                   // Keyword global memory handle

{
    VARIANTKEYS       varKey;               // Loop counter and the result
    LPVARARRAY_HEADER lpMemHdrKey = NULL;   // Ptr to key header
    LPAPLCHAR         lpMemKey;             // Ptr to key global memory data

    // Lock the memory to get a ptr to it
    lpMemHdrKey = MyGlobalLockVar (hGlbKey);

    // Skip over header and dimensions to the data
    lpMemKey = VarArrayDataFmBase (lpMemHdrKey);

    // Compare the keyword with valid ones
    for (varKey = 0; varKey < countof (aVariantKeyStr); varKey++)
    if (lstrcmpW (lpMemKey, aVariantKeyStr[varKey].lpKeyName) EQ 0)
        break;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbKey); lpMemHdrKey = NULL;

    return varKey;
} // End PrimOpVariantValKeyGlb


//***************************************************************************
//  $SaveAllSysVars
//
//  Save the old value of all System Vars
//***************************************************************************

void SaveAllSysVars
    (LPALLSYSVARS_STR lpAllSysVars,     // Ptr to ALLSYSVARS_STR struc
     LPPERTABDATA     lpMemPTD)         // Ptr to PerTabData global memory

{
    // ENUM_VARIANT_CT  :
    // Save the old value
    lpAllSysVars->fQuadCT  = GetQuadCT ();

    // ENUM_VARIANT_DT  :
    // Save the old value
    lpAllSysVars->cQuadDT  = GetQuadDT ();

    // ENUM_VARIANT_FPC :
    // Save the old value
    lpAllSysVars->uQuadFPC = GetQuadFPC ();

    // ENUM_VARIANT_IO  :
    // Save the old value
    lpAllSysVars->bQuadIO  = GetQuadIO ();

    // ENUM_VARIANT_LR  :
    // Save the old value
    lpAllSysVars->cQuadLR  = GetQuadLR ();

    // ENUM_VARIANT_PP  :
    // Save the old value
    lpAllSysVars->uQuadPP  = GetQuadPP ();

    // ENUM_VARIANT_RL  :
    // Save the old value
    lpAllSysVars->uQuadRL  = GetQuadRL ();

    // ENUM_VARIANT_EIG :
    // No old value to save

    // ENUM_VARIANT_POCH:
    // No old value to save

    // ENUM_VARIANT_HCM:
    // Save the old value of eHCMul
    lpAllSysVars->eHCMul   = lpMemPTD->eHCMul;

    // ENUM_VARIANT_SUB:
    // No old value to save
} // End SaveAllSysVars


//***************************************************************************
//  $SetAllSysVars
//
//  Set the new value of all System Vars
//***************************************************************************

void SetAllSysVars
    (ENUM_VARIANT     enumVarOpr,       // Enum of the target
     APLLONGEST       aplRhtOprItm,     // Immediate Num/Chr
     LPALLSYSVARS_STR lpAllSysVars,     // Ptr to ALLSYSVARS_STR struc
     LPPERTABDATA     lpMemPTD)         // Ptr to PerTabData global memory

{
    // Split cases based upon the enum for the first item numeric
    switch (enumVarOpr)
    {
        case ENUM_VARIANT_CT  :
            // Set the new value
            SetQuadCT  (*(LPAPLFLOAT) &aplRhtOprItm);

            break;

        case ENUM_VARIANT_DT  :
            // Set the new value
            SetQuadDT  (*(LPAPLCHAR ) &aplRhtOprItm);

            break;

        case ENUM_VARIANT_FPC :
            // Set the new value
            SetQuadFPC (*(LPAPLUINT ) &aplRhtOprItm);

            break;

        case ENUM_VARIANT_IO  :
            // Set the new value
            SetQuadIO  (*(LPAPLBOOL ) &aplRhtOprItm);

            break;

        case ENUM_VARIANT_LR  :
            // Set the new value
            SetQuadLR  (*(LPAPLCHAR ) &aplRhtOprItm);

            break;

        case ENUM_VARIANT_PP  :
            // Set the new value
            SetQuadPPV (*(LPAPLINT  ) &aplRhtOprItm);

            break;

        case ENUM_VARIANT_RL  :
            // Set the new value
            SetQuadRL  (*(LPAPLINT  ) &aplRhtOprItm);

            break;

        case ENUM_VARIANT_EIG :
            // Set the new value
            lpAllSysVars->uEigenNew = *(LPAPLUINT   ) &aplRhtOprItm;

            break;

        case ENUM_VARIANT_POCH:
            // Set the new value
            lpAllSysVars->uPochNew  = *(LPAPLUINT   ) &aplRhtOprItm;

            break;

        case ENUM_VARIANT_HCM:
            // Split cases based upon the incoming character
            switch (*(LPAPLCHAR) &aplRhtOprItm)
            {
                case L'i':
                    // Set the new value
                    lpMemPTD->eHCMul = ENUMHCM_INT;

                    break;

                case L'e':
                    // Set the new value
                    lpMemPTD->eHCMul = ENUMHCM_EXT;

                    break;

                case L'x':
                    // Set the new value
                    lpMemPTD->eHCMul = ENUMHCM_X;

                    break;

                case L'd':
                    // Set the new value
                    lpMemPTD->eHCMul = ENUMHCM_D;

                    break;

                case L'c':
                    // Set the new value
                    lpMemPTD->eHCMul = ENUMHCM_C;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ENUM_VARIANT_SUB:
            // Set the new value
            lpAllSysVars->cSubNew   = *(LPAPLCHAR) &aplRhtOprItm;

            break;

        case ENUM_VARIANT_GALL:
            break;

        defstop
            break;
    } // End SWITCH
} // End SetAllSysVars


//***************************************************************************
//  $RestAllSysVars
//
//  Restore the old value of all System Vars
//***************************************************************************

void RestAllSysVars
    (ENUM_VARIANT     enumVarOpr,       // Enum of the target
     LPALLSYSVARS_STR lpAllSysVars,     // Ptr to ALLSYSVARS_STR struc
     LPPERTABDATA     lpMemPTD)         // Ptr to PerTabData global memory

{
    // Split cases based upon the enum for the first item numeric
    switch (enumVarOpr)
    {
        case ENUM_VARIANT_CT  :
            // Restore the old value
            SetQuadCT (lpAllSysVars->fQuadCT);

            break;

        case ENUM_VARIANT_DT  :
            // Restore the old value
            SetQuadDT (lpAllSysVars->cQuadDT);

            break;

        case ENUM_VARIANT_FPC :
            // Restore the old value
            SetQuadFPC (lpAllSysVars->uQuadFPC);

            break;

        case ENUM_VARIANT_IO  :
            // Restore the old value
            SetQuadIO (lpAllSysVars->bQuadIO);

            break;

        case ENUM_VARIANT_LR  :
            // Restore the old value
            SetQuadLR (lpAllSysVars->cQuadLR);

            break;

        case ENUM_VARIANT_PP  :
            // Restore the old value
            SetQuadPPV (lpAllSysVars->uQuadPP);

            break;

        case ENUM_VARIANT_RL  :
            // Restore the old value
            SetQuadRL  (lpAllSysVars->uQuadRL);

            break;

        case ENUM_VARIANT_EIG :
            // No old value to restore

            break;

        case ENUM_VARIANT_POCH:
            // No old value to restore

            break;

        case ENUM_VARIANT_HCM:
            // Restore the old value
            lpMemPTD->eHCMul = lpAllSysVars->eHCMul;

            break;

        case ENUM_VARIANT_SUB:
            // No old value to restore

            break;

        case ENUM_VARIANT_GALL:
        case ENUM_VARIANT_UNK:          // Ignore this
            break;

        defstop
            break;
    } // End SWITCH
} // End RestAllSysVars


//***************************************************************************
//  $InitVarOprTab
//
//  Initialize all Variant operator table entries
//***************************************************************************

void InitVarOprTab
    (void)

{
    UINT uCnt;              // Loop counter

    // Loop through all of the entries
    for (uCnt = 0; uCnt < countof (varOprStr); uCnt++)
    {
        Assert (varOprTab[(*varOprStr[uCnt].SYMTRANS) (varOprStr[uCnt].wc)] EQ NULL);

        // Point to the entry based upon the function/operator symbol
        varOprTab[(*varOprStr[uCnt].SYMTRANS) (varOprStr[uCnt].wc)] = &varOprStr[uCnt];
    } // End FOR
} // End InitVarOprTab


//***************************************************************************
//  $varOprCT
//
//  Variant operator []CT value validation routine
//***************************************************************************

UBOOL varOprCT
    (LPAPLFLOAT lpValue)

{
    return
      ValidateFloatTest (lpValue              ,     // Ptr to the value to validate
                          DEF_MIN_QUADCT      ,     // Minimum value
                          DEF_MAX_QUADCT      ,     // Maximum ...
                          bRangeLimit.CT      );    // TRUE iff range limiting
} // End varOprCT


//***************************************************************************
//  $varOprEIG
//
//  Variant operator []EIG value validation routine
//***************************************************************************

UBOOL varOprEIG
    (LPAPLINT   lpValue)

{
    return
      ValidateIntegerTest (lpValue            ,     // Ptr to the value to validate
                           DEF_MIN_QUADEIG    ,     // Minimum value
                           DEF_MAX_QUADEIG    ,     // Maximum ...
                           FALSE              );    // TRUE iff range limiting
} // End varOprEIG


//***************************************************************************
//  $varOprDT
//
//  Variant operator []DT value validation routine
//***************************************************************************

UBOOL varOprDT
    (LPAPLCHAR  lpValue)

{
    return
      ValidateCharTest (lpValue               ,     // Ptr to the value to validate
                        DEF_QUADDT_CWS[0]     ,     // Default value
                        DEF_QUADDT_ALLOW      ,     // Ptr to vector of allowed values
                        lpValue               );    // Ptr to the return value if <lpValue[0] EQ WC_EOS>
} // End varOprDT


//***************************************************************************
//  $varOprFPC
//
//  Variant operator []FPC value validation routine
//***************************************************************************

UBOOL varOprFPC
    (LPAPLINT   lpValue)

{
    return
      ValidateIntegerTest (lpValue            ,     // Ptr to the value to validate
                           DEF_MIN_QUADFPC    ,     // Minimum value
                           DEF_MAX_QUADFPC    ,     // Maximum ...
                           bRangeLimit.FPC    );    // TRUE iff range limiting
} // End varOprFPC


//***************************************************************************
//  $varOprIO
//
//  Variant operator []IO value validation routine
//***************************************************************************

UBOOL varOprIO
    (LPAPLINT   lpValue)

{
    return
      ValidateIntegerTest (lpValue            ,     // Ptr to the value to validate
                           DEF_MIN_QUADIO     ,     // Minimum value
                           DEF_MAX_QUADIO     ,     // Maximum ...
                           bRangeLimit.IO     );    // TRUE iff range limiting
} // End varOprIO


//***************************************************************************
//  $varOprLR
//
//  Variant operator []LR value validation routine
//***************************************************************************

UBOOL varOprLR
    (LPAPLCHAR  lpValue)

{
    return
      ValidateCharTest (lpValue               ,     // Ptr to the value to validate
                        DEF_QUADLR_CWS[0]     ,     // Default value
                        DEF_QUADLR_ALLOW      ,     // Ptr to vector of allowed values
                        lpValue               );    // Ptr to the return value if <lpValue[0] EQ WC_EOS>
} // End varOprLR


//***************************************************************************
//  $varOprPP
//
//  Variant operator []PP value validation routine
//***************************************************************************

UBOOL varOprPP
    (LPAPLINT   lpValue)

{
    return
      ValidateIntegerTest (lpValue            ,     // Ptr to the value to validate
                           DEF_MIN_QUADPP     ,     // Minimum value
                           DEF_MAX_QUADPP_VFP ,     // Maximum ...
                           bRangeLimit.PP     );    // TRUE iff range limiting
} // End varOprPP


//***************************************************************************
//  $varOprRL
//
//  Variant operator []RL value validation routine
//***************************************************************************

UBOOL varOprRL
    (LPAPLINT   lpValue)

{
    return
      ValidateIntegerTest (lpValue            ,     // Ptr to the value to validate
                           DEF_MIN_QUADRL     ,     // Minimum value
                           DEF_MAX_QUADRL     ,     // Maximum ...
                           bRangeLimit.RL     );    // TRUE iff range limiting
} // End varOprRL


//***************************************************************************
//  $varOprPOCH
//
//  Variant operator []POCH value validation routine
//***************************************************************************

UBOOL varOprPOCH
    (LPAPLINT   lpValue)

{
    return TRUE;
} // End varOprPOCH


//***************************************************************************
//  $varOprGALL
//
//  Variant operator []GALL value validation routine
//***************************************************************************

UBOOL varOprGALL
    (LPAPLCHAR lpValue)

{
    WCHAR wszTemp[2];

    wszTemp[0] = lpValue[0];
    wszTemp[1] = WC_EOS;

    return (*CharLowerW (wszTemp) EQ L'a');
} // End varOprGALL


//***************************************************************************
//  $varOprHCM
//
//  Variant operator []HCM value validation routine
//***************************************************************************

UBOOL varOprHCM
    (LPAPLCHAR  lpValue)

{
    return
      ValidateCharTest (lpValue               ,     // Ptr to the value to validate
                        DEF_QUADHCM_CWS[0]    ,     // Default value
                        DEF_QUADHCM_ALLOW     ,     // Ptr to vector of allowed values
                        lpValue               );    // Ptr to the return value if <lpValue[0] EQ WC_EOS>
} // End varOprHCM


//***************************************************************************
//  $varOprSUB
//
//  Variant operator []SUB value validation routine
//***************************************************************************

UBOOL varOprSUB
    (LPAPLCHAR  lpValue)

{
    return
      ValidateCharTest (lpValue               ,     // Ptr to the value to validate
                        DEF_QUADSUB_CWS[0]    ,     // Default value
                        DEF_QUADSUB_ALLOW     ,     // Ptr to vector of allowed values
                        lpValue               );    // Ptr to the return value if <lpValue[0] EQ WC_EOS>
} // End varOprSUB


//***************************************************************************
//  End of File: po_variant.c
//***************************************************************************
