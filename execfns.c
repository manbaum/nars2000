
//***************************************************************************
//  NARS2000 -- Execution Functions
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
//  $ExecuteFn0
//
//  Execute a niladic function
//***************************************************************************

LPPL_YYSTYPE ExecuteFn0
    (LPPL_YYSTYPE lpYYFcn0)     // Ptr to function PL_YYSTYPE

{
    LPPRIMFNS lpNameFcn;

    // tkData is an LPSYMENTRY
    Assert (GetPtrTypeDir (lpYYFcn0->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

    lpNameFcn = lpYYFcn0->tkToken.tkData.tkSym->stData.stNameFcn;

    if (lpYYFcn0->tkToken.tkData.tkSym->stFlags.FcnDir)
        // Call the execution routine
        return (*lpNameFcn) (NULL,
                            &lpYYFcn0->tkToken,
                             NULL,
                             NULL);
    else
        // tkData is a valid HGLOBAL function array or user-defined function/operator
        Assert (IsGlbTypeFcnDir (lpNameFcn)
             || IsGlbTypeDfnDir (lpNameFcn));

        // Split cases based upon the array signature
        switch (GetSignatureGlb (lpNameFcn))
        {
            case FCNARRAY_HEADER_SIGNATURE:
                // Execute a function array global memory handle
                return ExecFcnGlb_EM_YY (NULL,          // Ptr to left arg token (may be NULL if monadic or niladic)
                                         lpNameFcn,     // Function array global memory handle
                                         NULL,          // Ptr to right arg token (may be NULL if niladic)
                                         NULL);         // Ptr to axis token (may be NULL)
            case DFN_HEADER_SIGNATURE:
                // Execute a user-defined function/operator global memory handle
                return ExecDfnGlb_EM_YY (lpNameFcn,     // User-defined function/operator global memory handle
                                         NULL,          // Ptr to left arg token (may be NULL if monadic)
                                         lpYYFcn0,      // Ptr to function strand
                                         NULL,          // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                         NULL,          // Ptr to right arg token
                                         LINENUM_ONE);  // Starting line # (see LINE_NUMS)
            defstop
                return NULL;
        } // End SWITCH
} // End ExecuteFn0


//***************************************************************************
//  $ExecFunc_EM_YY
//
//  Execute a user-defined, system, or primitive function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFunc_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFunc_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    LPPRIMFNS lpPrimFn;             // Ptr to direct primitive or system function
    LPTOKEN   lptkAxis;             // Ptr to axis token (may be NULL)
    HGLOBAL   hGlbFcn;              // Function array global memory handle

    DBGENTER;

    Assert (lpYYFcnStr->YYInuse);

    // Check for NoValue
    if (IsTokenNoValue (lptkLftArg)
     || IsTokenNoValue (lptkRhtArg))
        goto VALUE_EXIT;

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStr);

    // Split cases based upon the function token type
    switch (lpYYFcnStr->tkToken.tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            lpPrimFn = PrimFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
            if (!lpPrimFn)
                goto SYNTAX_EXIT;

            return (*lpPrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, NULL);

        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYFcnStr->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lpYYFcnStr->tkToken.tkData.tkSym->stFlags.Imm)
            {
                STFLAGS stFlags;

                // Get the STE flags of the first token
                stFlags = lpYYFcnStr->tkToken.tkData.tkSym->stFlags;

                // Skip assertion if it's some kind of function/operator
                if (IsNameTypeFnOp (stFlags.stNameType))
                {
                    LPPRIMFNS lpNameFcn;

                    // Get the address of the execution routine
                    lpNameFcn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stNameFcn;

                    // If it's an internal function, go there
                    if (stFlags.FcnDir)
                        return (*lpNameFcn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, NULL);

                    // Use the HGLOBAL
                    hGlbFcn = lpNameFcn;
                } else
                    // Use the HGLOBAL
                    hGlbFcn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL function array
                //   or user-defined function/operator
                Assert (IsGlbTypeFcnDir (hGlbFcn)
                     || IsGlbTypeDfnDir (hGlbFcn));

                // If it's a user-defined function/operator, ...
                if (stFlags.UsrDfn)
                    return ExecDfnGlb_EM_YY (hGlbFcn,       // User-defined function/operator global memory handle
                                             lptkLftArg,    // Ptr to left arg token (may be NULL if monadic)
                                             lpYYFcnStr,    // Ptr to function strand
                                             lptkAxis,      // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                             lptkRhtArg,    // Ptr to right arg token
                                             LINENUM_ONE);  // Starting line # (see LINE_NUMS)
                else
                    // Execute a function array global memory handle
                    return ExecFcnGlb_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if monadic or niladic)
                                             hGlbFcn,       // Function array global memory handle
                                             lptkRhtArg,    // Ptr to right arg token (may be NULL if niladic)
                                             NULL);         // Ptr to axis token (may be NULL)
            } // End IF

            // Handle the immediate case

            // Split cases based upon the immediate type
            switch (lpYYFcnStr->tkToken.tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_PRIMFCN:
                {
                    TOKEN tkFn = {0};

                    lpPrimFn = PrimFnsTab[FcnTrans (lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar)];
                    if (!lpPrimFn)
                        goto SYNTAX_EXIT;

                    // Fill in for PrimFn*** test
                    tkFn.tkFlags.TknType   = TKT_FCNIMMED;
                    tkFn.tkFlags.ImmType   = GetImmTypeFcn (lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar);
////////////////////tkFn.tkFlags.NoDisplay = FALSE; // Already zero from {0}
                    tkFn.tkData.tkChar     = lpYYFcnStr->tkToken.tkData.tkSym->stData.stChar;
                    tkFn.tkCharIndex       = lpYYFcnStr->tkToken.tkCharIndex;

                    return (*lpPrimFn) (lptkLftArg, &tkFn, lptkRhtArg, NULL);
                } // End IMMTYPE_PRIMFCN

                defstop
                    break;
            } // End SWITCH

        case TKT_FCNARRAY:
            // Get the HGLOBAL
            hGlbFcn = lpYYFcnStr->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL function array
            //   or user-defined function/operator
            Assert (IsGlbTypeFcnDir (hGlbFcn)
                 || IsGlbTypeDfnDir (hGlbFcn));

            // If it's a user-defined function/operator, ...
            switch (GetSignatureGlb (hGlbFcn))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Execute a function array global memory handle
                    return ExecFcnGlb_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if monadic or niladic)
                                             hGlbFcn,       // Function array global memory handle
                                             lptkRhtArg,    // Ptr to right arg token
                                             lptkAxis);     // Ptr to axis token (may be NULL)
                case DFN_HEADER_SIGNATURE:
                    // Execute a user-defined function/operator global memory handle
                    return ExecDfnGlb_EM_YY (hGlbFcn,       // User-defined function/operator global memory handle
                                             lptkLftArg,    // Ptr to left arg token (may be NULL if monadic)
                                             lpYYFcnStr,    // Ptr to function strand
                                             lptkAxis,      // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                             lptkRhtArg,    // Ptr to right arg token
                                             LINENUM_ONE);  // Starting line # (see LINE_NUMS)
                defstop
                    break;
            } // End SWITCH

        defstop
            break;
    } // End SWITCH

    return NULL;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    return NULL;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                               (lptkLftArg NE NULL) ? lptkLftArg
                                                    : lptkRhtArg);
    return NULL;
} // End ExecFunc_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecFcnGlb_EM_YY
//
//  Execute a function in global memory
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFcnGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFcnGlb_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     HGLOBAL hGlbFcn,               // Handle to function strand
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPFCNARRAY_HEADER lpHeader;     // Ptr to function array header
    NAME_TYPES        fnNameType;   // Function array NAMETYPE
    UINT              tknNELM;      // # tokens in the function array
    LPPL_YYSTYPE      lpYYFcnStr,   // Ptr to function strand
                      lpYYRes;      // Ptr to the result
    LPTOKEN           lptkAxis2;    // Ptr to secondary axis token (may be NULL)

    // Clear the ptr type bits
    hGlbFcn = ClrPtrTypeDir (hGlbFcn);

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbFcn);

    // Save the NAMETYPE_xxx and # tokens
    fnNameType = lpHeader->fnNameType;
    tknNELM    = lpHeader->tknNELM;

    // Skip over the header to the data (PL_YYSTYPEs)
    lpYYFcnStr = FcnArrayBaseToData (lpHeader);

    // Check for axis operator
    lptkAxis2 = CheckAxisOper (lpYYFcnStr);

    // If two axis tokens, that's an error
    if (lptkAxis && lptkAxis2)
        goto AXIS_SYNTAX_EXIT;
    else
    // If the secondary only, use it
    if (lptkAxis2)
        lptkAxis = lptkAxis2;

    // If it's a Train, ...
    if (fnNameType EQ NAMETYPE_TRN)
    {
        if (lptkAxis NE NULL)
            lpYYRes = PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken);
        else
            lpYYRes = ExecTrain_EM_YY   (lptkLftArg,    // Ptr to left arg token
                                         lpYYFcnStr,    // Ptr to function strand
                                         lptkRhtArg,    // Ptr to right arg token
                                         tknNELM);      // # elements in the train
    } else
        // The contents of the global memory object consist of
        //   a series of PL_YYSTYPEs in RPN order.
        lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStr,    // Ptr to function strand
                                     lptkRhtArg,    // Ptr to right arg token
                                     lptkAxis);     // Ptr to axis token
    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcn); lpYYFcnStr = NULL;

    return lpYYRes;
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    return NULL;
} // End ExecFcnGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecTrain_EM_YY
//
//  Execute a Train
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecTrain_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecTrain_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     UINT         tknNELM)          // # elements in the Train

{
    LPPL_YYSTYPE lpYYRes  = NULL,   // Ptr to the result
                 lpYYRes1 = NULL,   // Ptr to temporary result #1
                 lpYYRes2 = NULL;   // ...                      2

    // Note that the rightmost function is at the start of Train (lpYYFcnStr[0])
    //   and the leftmost function is at the end of the Train (lpYYFcnStr[tknNELM - 1]).

    Assert (tknNELM > 1);

    // Split cases based upon the # elements in the Train
    switch (tknNELM)
    {               //  Monadic             Dyadic
                    //  ---------------------------
        case 2:     //  R g h R             L g h R
            // Execute the righthand function
            //   on the right arg
            lpYYRes1 = ExecFuncStr_EM_YY (NULL,             // Ptr to left arg token
                                         &lpYYFcnStr[0],    // Ptr to function strand
                                          lptkRhtArg,       // Ptr to right arg token
                                          NULL);            // Ptr to axis token
            if (lpYYRes1)
            {
#ifdef DEBUG
                // Decrement the SI level of lpYYRes1
                //   so YYResIsEmpty won't complain
                lpYYRes1->SILevel--;
#endif
                // If this derived function is monadic, ...
                if (lptkLftArg EQ NULL)
                    lptkLftArg = lptkRhtArg;

                // Execute the lefthand function between
                //   the left arg and the previous result
                lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,        // Ptr to left arg token
                                            &lpYYFcnStr[1],     // Ptr to function strand
                                            &lpYYRes1->tkToken, // Ptr to right arg token
                                             NULL);             // Ptr to axis token
#ifdef DEBUG
                // Restore the SI level of lpYYRes1
                lpYYRes1->SILevel++;
#endif
            } // End IF

            break;

        case 3:     //  (f R) g (h R)       (L f R) g (L h R)
            // Execute the righthand function between
            //   the left and right args
            lpYYRes1 = ExecFuncStr_EM_YY (lptkLftArg,       // Ptr to left arg token
                                         &lpYYFcnStr[0],    // Ptr to function strand
                                          lptkRhtArg,       // Ptr to right arg token
                                          NULL);            // Ptr to axis token
            if (lpYYRes1)
            {
#ifdef DEBUG
                // Decrement the SI level of lpYYRes1
                //   so YYResIsEmpty won't complain
                lpYYRes1->SILevel--;
#endif
                // Execute the lefthand function between
                //   the left and right args
                lpYYRes2 = ExecFuncStr_EM_YY (lptkLftArg,       // Ptr to left arg token
                                             &lpYYFcnStr[2],    // Ptr to function strand
                                              lptkRhtArg,       // Ptr to right arg token
                                              NULL);            // Ptr to axis token
                if (lpYYRes2)
                {
#ifdef DEBUG
                    // Decrement the SI level of lpYYRes2
                    //   so YYResIsEmpty won't complain
                    lpYYRes2->SILevel--;
#endif
                    // Execute the middle function between
                    //   the two previous results
                    lpYYRes = ExecFuncStr_EM_YY (&lpYYRes2->tkToken,    // Ptr to left arg token
                                                 &lpYYFcnStr[1],        // Ptr to function strand
                                                 &lpYYRes1->tkToken,    // Ptr to right arg token
                                                  NULL);                // Ptr to axis token
#ifdef DEBUG
                    // Restore the SI level of lpYYRes2
                    lpYYRes2->SILevel++;
#endif
                } // End IF
#ifdef DEBUG
                // Restore the SI level of lpYYRes1
                lpYYRes1->SILevel++;
#endif
            } // End IF

            break;

        default:
            // If the # elements in the Train is Odd, ...
            if (BIT0 & tknNELM)
            {
                // Execute all but the two leftmost functions as a Train
                //   between the left and right args
                lpYYRes1 = ExecTrain_EM_YY (lptkLftArg,     // Ptr to left arg token
                                           &lpYYFcnStr[0],  // Ptr to function strand
                                            lptkRhtArg,     // Ptr to right arg token
                                            tknNELM - 2);   // # elements in the Train

                if (lpYYRes1)
                {
#ifdef DEBUG
                    // Decrement the SI level of lpYYRes1
                    //   so YYResIsEmpty won't complain
                    lpYYRes1->SILevel--;
#endif
                    // Execute the leftmost function
                    //   between the left and right args
                    lpYYRes2 = ExecFuncStr_EM_YY (lptkLftArg,               // Ptr to left arg token
                                                 &lpYYFcnStr[tknNELM - 1],  // Ptr to function strand
                                                  lptkRhtArg,               // Ptr to right arg token
                                                  NULL);                    // Ptr to axis token
                    if (lpYYRes2)
                    {
#ifdef DEBUG
                        // Decrement the SI level of lpYYRes2
                        //   so YYResIsEmpty won't complain
                        lpYYRes2->SILevel--;
#endif
                        // Execute the next to the leftmost function
                        //   between the two previous results
                        lpYYRes = ExecFuncStr_EM_YY (&lpYYRes2->tkToken,        // Ptr to left arg token
                                                     &lpYYFcnStr[tknNELM - 2],  // Ptr to function strand
                                                     &lpYYRes1->tkToken,        // Ptr to right argtoken
                                                      NULL);                    // Ptr to axis token
#ifdef DEBUG
                        // Restore the SI level of lpYYRes2
                        lpYYRes2->SILevel++;
#endif
                    } // End IF
#ifdef DEBUG
                    // Restore the SI level of lpYYRes1
                    lpYYRes1->SILevel++;
#endif
                } // End IF
            } else
            // If the # elements in the Train is Even, ...
            {
                // If this derived function is monadic, ...
                if (lptkLftArg EQ NULL)
                    lptkLftArg = lptkRhtArg;

                // Execute all but the leftmost function as a Train
                //   on the right arg
                lpYYRes1 = ExecTrain_EM_YY (NULL,           // Ptr to left arg token
                                           &lpYYFcnStr[0],  // Ptr to function strand
                                            lptkRhtArg,     // Ptr to right arg token
                                            tknNELM - 1);   // # elements in the Train
                if (lpYYRes1)
                {
#ifdef DEBUG
                    // Decrement the SI level of lpYYRes1
                    //   so YYResIsEmpty won't complain
                    lpYYRes1->SILevel--;
#endif
                    // Execute the leftmost function between
                    //   the left arg and the previous result
                    lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,                // Ptr to left arg token
                                                &lpYYFcnStr[tknNELM - 1],   // Ptr to function strand
                                                &lpYYRes1->tkToken,         // Ptr to right arg token
                                                 NULL);                     // Ptr to axis token
#ifdef DEBUG
                    // Restore the SI level of lpYYRes1
                    lpYYRes1->SILevel++;
#endif
                } // End IF
            } // End IF/ELSE

            break;
    } // End SWITCH

    // If a previous result is active, ...
    if (lpYYRes1)
    {
        // Free the previous result
        FreeResult (&lpYYRes1->tkToken); YYFree (lpYYRes1); lpYYRes1 = NULL;
    } // End IF

    if (lpYYRes2)
    {
        // Free the previous result
        FreeResult (&lpYYRes2->tkToken); YYFree (lpYYRes2); lpYYRes2 = NULL;
    } // End IF

    return lpYYRes;
} // End ExecTrain_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecFuncStr_EM_YY
//
//  Execute a function strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFuncStr_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFuncStr_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxis)         // Ptr to axis token (may be NULL)

{
    return
      ExecFuncStrLine_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if monadic)
                             lpYYFcnStr,    // Ptr to function strand
                             lptkRhtArg,    // Ptr to right arg token
                             lptkAxis,      // Ptr to axis token (may be NULL)
                             LINENUM_ONE);  // Starting line # type (see LINE_NUMS)
} // End ExecFuncStr_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecFuncStrLine_EM_YY
//
//  Execute a function strand at a given line # type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecFuncStrLine_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecFuncStrLine_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxis,         // Ptr to axis token (may be NULL)
     LINE_NUMS    startLineNum)     // Starting line # (see LINE_NUMS)

{
    LPPRIMFNS lpPrimFn;             // Ptr to function address
    HGLOBAL   hGlbFcn;              // Function strand or user-defined function/operator global memory handle

    // Note that lptkAxis is not NULL for (say) L {take}[1]{each} R

    // Split cases based upon the type of the first token
    switch (lpYYFcnStr->tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
            return ExecOp1_EM_YY
                   (lptkLftArg,     // Ptr to left arg token
                    lpYYFcnStr,     // Ptr to operator function strand
                    lptkRhtArg);    // Ptr to right arg token

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
        case TKT_OPJOTDOT:
            return ExecOp2_EM_YY
                   (lptkLftArg,     // Ptr to left arg token
                    lpYYFcnStr,     // Ptr to operator function strand
                    lptkRhtArg);    // Ptr to right arg token

        case TKT_FCNIMMED:  // Either F or F[X]
            Assert (lpYYFcnStr->TknCount EQ 1
                 || lpYYFcnStr->TknCount EQ 2);

            // Check for axis operator
            lptkAxis = CheckAxisOper (lpYYFcnStr);

            lpPrimFn = PrimFnsTab[SymTrans (&lpYYFcnStr->tkToken)];
            if (!lpPrimFn)
                goto SYNTAX_EXIT;

            return (*lpPrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, lptkAxis);

        case TKT_FCNARRAY:
                                // 1.  User-defined operator
                                //   e.g., Z{is}L (F FOO G) R
                                //         +foo- 1 2
                                // 2.  User-defined function
                                //   e.g., Z{is}FOO R
                                //         3{jot}foo 1 2
                                // 3.  User-defined function/operator w/axis operator
                                //   e.g., Z{is}FOO[X] R
                                //         foo[1] 1 2
                                // 4.  Monadic operator derived function
                                //   e.g., f{is}/[1]
                                //         +f 2 3{rho}{iota}6
            // Get the global memory handle
            hGlbFcn = lpYYFcnStr->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL function array
            //   or user-defined function/operator
            Assert (IsGlbTypeFcnDir (hGlbFcn)
                 || IsGlbTypeDfnDir (hGlbFcn));

            // Split cases based upon the array signature
            switch (GetSignatureGlb (hGlbFcn))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Check for axis operator
                    lptkAxis = CheckAxisOper (lpYYFcnStr);

                    // Execute a function array global memory handle
                    return ExecFcnGlb_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if monadic or niladic)
                                             hGlbFcn,       // Function array global memory handle
                                             lptkRhtArg,    // Ptr to right arg token
                                             lptkAxis);     // Ptr to axis token (may be NULL)
                case DFN_HEADER_SIGNATURE:
                    // Execute a user-defined function/operator global memory handle
                    return ExecDfnGlb_EM_YY (hGlbFcn,       // User-defined function/operator global memory handle
                                             lptkLftArg,    // Ptr to left arg token (may be NULL if monadic)
                                             lpYYFcnStr,    // Ptr to function strand
                                             lptkAxis,      // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                             lptkRhtArg,    // Ptr to right arg token
                                             startLineNum); // Starting line # (see LINE_NUMS)
                defstop
                    break;
            } // End SWITCH

            DbgStop ();             // We should never get here

        case TKT_FCNNAMED:
            Assert (lpYYFcnStr->TknCount EQ 1
                 || lpYYFcnStr->TknCount EQ 2);

            // Check for axis operator
            lptkAxis = CheckAxisOper (lpYYFcnStr);

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYFcnStr->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Get the global memory handle or function address if direct
            hGlbFcn  =
            lpPrimFn = lpYYFcnStr->tkToken.tkData.tkSym->stData.stNameFcn;

            // Check for internal functions
            if (lpYYFcnStr->tkToken.tkData.tkSym->stFlags.FcnDir)
                return (*lpPrimFn) (lptkLftArg, &lpYYFcnStr->tkToken, lptkRhtArg, lptkAxis);

            // tkData is a valid HGLOBAL function array
            //   or user-defined function/operator
            Assert (IsGlbTypeFcnDir (hGlbFcn)
                 || IsGlbTypeDfnDir (hGlbFcn));

            // Split cases based upon the array signature
            switch (GetSignatureGlb (hGlbFcn))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Execute a function array global memory handle
                    return ExecFcnGlb_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if monadic or niladic)
                                             hGlbFcn,       // Function array global memory handle
                                             lptkRhtArg,    // Ptr to right arg token
                                             lptkAxis);     // Ptr to axis token (may be NULL)
                case DFN_HEADER_SIGNATURE:
                    // Execute a user-defined function/operator global memory handle
                    return ExecDfnGlb_EM_YY (hGlbFcn,       // User-defined function/operator global memory handle
                                             lptkLftArg,    // Ptr to left arg token (may be NULL if monadic)
                                             lpYYFcnStr,    // Ptr to function strand
                                             lptkAxis,      // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                                             lptkRhtArg,    // Ptr to right arg token
                                             startLineNum); // Starting line # (see LINE_NUMS)
                defstop
                    break;
            } // End SWITCH

        defstop
            goto ERROR_EXIT;
    } // End SWITCH

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
ERROR_EXIT:
    return NULL;
} // End ExecFuncStrLine_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecOp1_EM_YY
//
//  Execute a monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecOp1_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecOp1_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    // Split cases based upon the type of the monadic operator
    switch (lpYYFcnStrOpr->tkToken.tkData.tkChar)
    {
        case UTF16_SLASH:           // Reduction along the last coordinate
        case UTF16_SLASHBAR:        // Reduction along the first coordinate
        case INDEX_OPSLASH:         // Reduction along the last coordinate
        case INDEX_OPSLASHBAR:      // Reduction along the first coordinate
            return PrimOpSlash_EM_YY (lptkLftArg,   // Ptr to left arg token (may be NULL if monadic)
                                      lpYYFcnStrOpr,// Ptr to operator function strand
                                      lptkRhtArg);  // Ptr to right arg token
        case UTF16_SLOPE:           // Scan along the last coordinate
        case UTF16_SLOPEBAR:        // Scan along the first coordinate
        case INDEX_OPSLOPE:         // Scan along the last coordinate
        case INDEX_OPSLOPEBAR:      // Scan along the first coordinate
            return PrimOpSlope_EM_YY (lptkLftArg,               // Ptr to left arg token (may be NULL if monadic)
                                      lpYYFcnStrOpr,            // Ptr to operator function strand
                                      lptkRhtArg);              // Ptr to right arg token
        case UTF16_STILETILDE:      // Partition
            return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken);

            DbgBrk ();              // ***FINISHME*** -- UTF16_STILETILDE






            return NULL;

        case UTF16_DIERESIS:        // Each
            return PrimOpDieresis_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
                                         lpYYFcnStrOpr,         // Ptr to operator function strand
                                         lptkRhtArg);           // Ptr to right arg token
        case UTF16_DIERESISTILDE:   // Commute/Duplicate
            return PrimOpDieresisTilde_EM_YY (lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
                                              lpYYFcnStrOpr,    // Ptr to operator function strand
                                              lptkRhtArg);      // Ptr to right arg token
        case UTF16_CIRCLEMIDDLEDOT: // Null Op
            return PrimOpCircleMiddleDot_EM_YY (lptkLftArg,     // Ptr to left arg token (may be NULL if monadic)
                                                lpYYFcnStrOpr,  // Ptr to operator function strand
                                                lptkRhtArg);    // Ptr to right arg token
        defstop
            return NULL;
    } // End SWITCH
} // End ExecOp1_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecOp2_EM_YY
//
//  Execute a dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecOp2_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecOp2_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    LPTOKEN lptkAxis;                   // Ptr to axis token (may be NULL)

    // Check for axis operator
    lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    // Split cases based upon the type of the dyadic operator
    switch (lpYYFcnStrOpr->tkToken.tkData.tkChar)
    {
        case UTF16_JOTDOT:          // Outer product
            return PrimOpJotDot_EM_YY (lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg);         // Ptr to right arg token
        case UTF16_DOT:             // Inner product
            return PrimOpDot_EM_YY    (lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg);         // Ptr to right arg token
        case UTF16_JOT:             // Compose
            return PrimOpJot_EM_YY    (lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
                                       lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg);         // Ptr to right arg token
        case UTF16_DIERESISDEL:     // Dual
            return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken);

            DbgBrk ();              // ***FINISHME*** -- UTF16_DIERESISDEL






            return NULL;

        case INDEX_DIERESISJOT:     // Rank (as dyadic op)
            return PrimOpDieresisJot_EM_YY (lptkLftArg,     // Ptr to left arg token (may be NULL if monadic)
                                            lpYYFcnStrOpr,  // Ptr to operator function strand
                                            lptkRhtArg);    // Ptr to right arg token
        case UTF16_DIERESISDOWNTACK:// Convolution
            return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken);

            DbgBrk ();              // ***FINISHME*** -- UTF16_DIERESISDOWNTACK






            return NULL;

        case UTF16_DIERESISSTAR:    // Power
            return PrimFnNonceError_EM (&lpYYFcnStrOpr->tkToken);

            DbgBrk ();              // ***FINISHME*** -- UTF16_DIERESISSTAR






            return NULL;

        defstop
            return NULL;
    } // End SWITCH
} // End ExecOp2_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: execfns.c
//***************************************************************************
