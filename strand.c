//***************************************************************************
//  NARS2000 -- Strand Routines
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

#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "fh_parse.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


/*  Strand Test cases
    -----------------

    0
    1
    2
    'a'
    'a' 'b'
    'ab'
    1 'a'
    1 'ab'
    1('a')
    1('ab')
    1 2.2
    3(4(5 6)7)8
    a {is} 1.5     {diamond} a 1.5
    a {is} 1.5 2.3 {diamond} a 1.5



 */

//***************************************************************************
//  $InitVarStrand
//
//  Initialize the strand stack.
//***************************************************************************

void InitVarStrand
    (LPPL_YYSTYPE lpYYArg)          // Ptr to the incoming argument

{
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Set the base of this strand to the next available location
    lpYYArg->lpYYStrandBase                   =
    lpplLocalVars->lpYYStrandBase[STRAND_VAR] = lpplLocalVars->lpYYStrandNext[STRAND_VAR];
} // End InitVarStrand


//***************************************************************************
//  $PushVarStrand_YY
//
//  Push a variable token onto the strand stack.
//***************************************************************************

LPPL_YYSTYPE PushVarStrand_YY
    (LPPL_YYSTYPE lpYYArg)          // Ptr to the incoming argument

{
    LPPL_YYSTYPE  lpYYRes;          // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_STRAND;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = NEG1U;         // Debug value
    lpYYRes->tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;
    lpYYRes->TknCount                  = 0;
    lpYYRes->lpYYFcnBase               = NULL;

    // Copy the strand base to the result
    lpYYRes->lpYYStrandBase  =
    lpYYArg->lpYYStrandBase  = lpplLocalVars->lpYYStrandBase[STRAND_VAR];

    // Save this token on the strand stack
    //   and skip over it
    YYCopyFreeDst (lpplLocalVars->lpYYStrandNext[STRAND_VAR]++, lpYYArg);
#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (STRAND_VAR);
#endif
    return lpYYRes;
} // End PushVarStrand_YY


//***************************************************************************
//  $PushFcnStrand_YY
//
//  Push a function token onto the strand stack.
//  No RefCnt copies aer made by this routine, so don't free
//    the argument.
//***************************************************************************

LPPL_YYSTYPE PushFcnStrand_YY
    (LPPL_YYSTYPE lpYYArg,          // Ptr to the incoming argument
     int          TknCount,         // Token count
     UBOOL        bIndirect)        // TRUE iff lpYYArg is indirect

{
    LPPL_YYSTYPE  lpYYRes,          // Ptr to the result
                  lpYYCopy;         // Ptr to local copy
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    lpYYArg->TknCount   = TknCount;
    lpYYArg->YYIndirect = bIndirect;

    // Copy the strand base to the result
    lpYYArg->lpYYStrandBase = lpplLocalVars->lpYYStrandBase[STRAND_FCN];
    if (!lpYYArg->lpYYFcnBase)
        lpYYArg->lpYYFcnBase = lpplLocalVars->lpYYStrandNext[STRAND_FCN];

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    YYCopy (lpYYRes, lpYYArg);

    // Return our own position so the next user
    //   of this token can refer to it.
    lpYYRes->lpYYFcnBase = lpplLocalVars->lpYYStrandNext[STRAND_FCN];

    // Save this token on the strand stack
    //   and skip over it
    lpYYCopy = CopyPL_YYSTYPE_EM_YY (lpYYArg, FALSE);
    YYCopyFreeDst (lpplLocalVars->lpYYStrandNext[STRAND_FCN]++, lpYYCopy);
    YYFree (lpYYCopy); lpYYCopy = NULL;
#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (STRAND_FCN);
#endif
    return lpYYRes;
} // End PushFcnStrand_YY


//***************************************************************************
//  $StripStrand
//
//  Strip a strand from the strand stack
//***************************************************************************

void StripStrand
    (LPPL_YYSTYPE lpYYStrand,       // Ptr to base of strand to strip
     int          strType)          // Strand type (STRAND_VAR or STRAND_FCN)

{
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // If we're not back at the beginning, set the new base
    //   to the base of the token previous to the current base
    if (lpplLocalVars->lpYYStrandBase[strType] NE lpplLocalVars->lpYYStrandStart[strType])
        lpplLocalVars->lpYYStrandBase[strType] =  lpplLocalVars->lpYYStrandBase[strType - STRAND_LEN];

    // Set next available slot to this YYtoken's base
    lpplLocalVars->lpYYStrandNext[strType] = lpYYStrand->lpYYStrandBase;

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (strType);
#endif
} // End StripStrand


//***************************************************************************
//  $FreeStrand
//
//  Free the tokens on the strand stack
//***************************************************************************

void FreeStrand
    (LPPL_YYSTYPE lpYYStrandNext,       // Ptr to next strand element
     LPPL_YYSTYPE lpYYStrand)           // Ptr to strand base

{
    int          iLen;
    LPPL_YYSTYPE lpYYToken;

    // Get the # elements in the strand
    iLen = (UINT) (lpYYStrandNext - lpYYStrand);

    for (lpYYToken = &lpYYStrand[iLen - 1];
         lpYYToken NE &lpYYStrand[-1];
         lpYYToken--)
    {
        switch (lpYYToken->tkToken.tkFlags.TknType)
        {
            case TKT_VARNAMED:
            case TKT_FCNNAMED:
            case TKT_OP1NAMED:
            case TKT_OP2NAMED:
            case TKT_OP3NAMED:
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                // Does it have a value?
                if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Value)
                    break;

                if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                {
                    // stData is an internal function, a valid HGLOBAL variable or function array,
                    //   or user-defined function/operator
                    Assert (lpYYToken->tkToken.tkData.tkSym->stFlags.FcnDir
                         || IsGlbTypeVarDir (lpYYToken->tkToken.tkData.tkSym->stData.stGlbData)
                         || IsGlbTypeFcnDir (lpYYToken->tkToken.tkData.tkSym->stData.stGlbData)
                         || IsGlbTypeDfnDir (lpYYToken->tkToken.tkData.tkSym->stData.stGlbData));
                } // End IF

                break;          // Don't free names

            case TKT_CHRSTRAND:
            case TKT_NUMSTRAND:
            case TKT_VARARRAY:
            case TKT_AXISARRAY:
                // Check for reused ptrs
                if (!PtrReusedDir (lpYYToken->tkToken.tkData.tkGlbData))
                {
                    // tkData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.tkGlbData));

                    if (FreeResultGlobalVar (lpYYToken->tkToken.tkData.tkGlbData))
                    {
#ifdef DEBUG_ZAP
                        dprintfW (L"**Zapping in FreeStrand: %p (%S#%d)",
                                  ClrPtrTypeDir (lpYYToken->tkToken.tkData.tkGlbData),
                                  FNLN);
#endif
                        lpYYToken->tkToken.tkData.tkGlbData = NULL;
                    } // End IF
                } // End IF

                break;

            case TKT_FCNARRAY:
                // Check for reused ptrs
                if (!PtrReusedDir (lpYYToken->tkToken.tkData.tkGlbData))
                {
                    // tkData is a valid HGLOBAL function array
                    Assert (IsGlbTypeFcnDir (lpYYToken->tkToken.tkData.tkGlbData));

                    if (FreeResultGlobalFcn (lpYYToken->tkToken.tkData.tkGlbData))
                    {
#ifdef DEBUG_ZAP
                        dprintfW (L"**Zapping in FreeStrand: %p (%S#%d)",
                                  ClrPtrTypeDir (lpYYToken->tkToken.tkData.tkGlbData),
                                  FNLN);
#endif
                        lpYYToken->tkToken.tkData.tkGlbData = NULL;
                    } // End IF
                } // End IF

                break;

            case TKT_AXISIMMED:
            case TKT_VARIMMED:
            case TKT_FCNIMMED:
            case TKT_OP1IMMED:
            case TKT_OP2IMMED:
            case TKT_OP3IMMED:
            case TKT_OPJOTDOT:
            case TKT_LISTSEP:
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR
} // End FreeStrand


//***************************************************************************
//  $MakeVarStrand_EM_YY
//
//  Make the variable strand into an immediate token or a global memory array.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeVarStrand_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakeVarStrand_EM_YY
    (LPPL_YYSTYPE lpYYArg)              // Ptr to incoming token

{
    int          iLen,                  // Length of the strand
                 iNELM,                 // # elements in the strand
                 iBitIndex;
    APLUINT      ByteRes;               // # bytes in the result
    LPPL_YYSTYPE lpYYToken,
                 lpYYStrand;
    HGLOBAL      hGlbStr,
                 hGlbData,
                 hGlbNum;
    LPVOID       lpMemStr,
                 lpMemNum;
    union tagLPAPL
    {
        LPAPLBOOL   Bool;
        LPAPLINT    Int;
        LPAPLCHAR   Char;
        LPAPLFLOAT  Float;
        LPAPLHETERO Hetero;
        LPAPLNESTED Nested;
        LPSYMENTRY  *Sym;
    } LPAPL;

static char tabConvert[][STRAND_LENGTH] =
// Initial       Boolean        Integer        Float          Char           CharStrand     String         Hetero         Nested
{{STRAND_INIT  , STRAND_BOOL  , STRAND_INT   , STRAND_FLOAT , STRAND_CHAR  , STRAND_CHARST, STRAND_STRING, STRAND_HETERO, STRAND_NESTED}, // Initial
 {STRAND_BOOL  , STRAND_BOOL  , STRAND_INT   , STRAND_FLOAT , STRAND_HETERO, STRAND_HETERO, STRAND_NESTED, STRAND_HETERO, STRAND_NESTED}, // Boolean
 {STRAND_INT   , STRAND_INT   , STRAND_INT   , STRAND_FLOAT , STRAND_HETERO, STRAND_HETERO, STRAND_NESTED, STRAND_HETERO, STRAND_NESTED}, // Integer
 {STRAND_FLOAT , STRAND_FLOAT , STRAND_FLOAT , STRAND_FLOAT , STRAND_HETERO, STRAND_HETERO, STRAND_NESTED, STRAND_HETERO, STRAND_NESTED}, // Float
 {STRAND_CHAR  , STRAND_HETERO, STRAND_HETERO, STRAND_HETERO, STRAND_CHARST, STRAND_CHARST, STRAND_NESTED, STRAND_HETERO, STRAND_NESTED}, // Char
 {STRAND_CHARST, STRAND_HETERO, STRAND_HETERO, STRAND_HETERO, STRAND_CHARST, STRAND_CHARST, STRAND_NESTED, STRAND_HETERO, STRAND_NESTED}, // CharStrand
 {STRAND_STRING, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED}, // String
 {STRAND_HETERO, STRAND_HETERO, STRAND_HETERO, STRAND_HETERO, STRAND_HETERO, STRAND_HETERO, STRAND_NESTED, STRAND_HETERO, STRAND_NESTED}, // Hetero
 {STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED, STRAND_NESTED}, // Nested
};

    char          cStrandCurType = STRAND_INIT,
                  cStrandNxtType;
    APLSTYPE      aplTypeNum,           // Numeric strand storage type
                  aplTypeRes;           // Result storage type
    IMM_TYPES     immTypeNum;           // Numeric strand immediate type
    APLNELM       aplNELMNum,           // Numeric strand NELM
                  uNum;                 // Loop counter
    APLLONGEST    aplLongestNum;        // Numeric strand immediate value
    LPSYMENTRY    lpSymEntry;           // Temporary STE
    UBOOL         bRet = TRUE;          // TRUE iff the result is valid
    LPPL_YYSTYPE  lpYYRes;              // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;        // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // The strand needs to be saved to global memory

    // Save the base of this strand
    lpYYStrand              =
    lpYYRes->lpYYStrandBase = lpYYArg->lpYYStrandBase;
    lpYYRes->lpYYFcnBase = (LPPL_YYSTYPE) -1;  // For debugging

    // Get the # elements in the strand
    iNELM = (UINT) (lpplLocalVars->lpYYStrandNext[STRAND_VAR] - lpYYStrand);

    // Initialize the length of the strand
    // This value may be larger than iNELM if there are TKT_NUMSTRAND tokens
    iLen = iNELM;

    // Trundle through the strand stack converting
    //   to a common memory allocation type
    // Note that, although it doesn't make any difference here,
    //   we look at the elements in the reverse
    //   order they are on the stack because
    //   we parsed the tokenization from right
    //   to left.
    for (lpYYToken = &lpYYStrand[iNELM - 1];
         bRet && lpYYToken NE &lpYYStrand[-1];
         lpYYToken--)
    {
        // Split cases based upon the token type
        switch (lpYYToken->tkToken.tkFlags.TknType)
        {
            case TKT_VARNAMED:
                // tkData is LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                // Check for VALUE ERROR
                if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Value)
                {
                    // If this is a single NoValue item, ignore it
                    //   as it's coming from a function with no return value
                    if (IsSingleton (iNELM)
                     && IsSymNoValue (lpYYToken->tkToken.tkData.tkSym))
                    {
                        cStrandNxtType = STRAND_BOOL;

                        break;
                    } // End IF

                    goto VALUE_EXIT;
                } // End IF

                if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                {
                    // Get the global handle
                    hGlbData = lpYYToken->tkToken.tkData.tkSym->stData.stGlbData;

                    // stData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir (hGlbData));

                    // Lock the memory to get a ptr to it
                    lpMemStr = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbData));

#define lpHeader    ((LPVARARRAY_HEADER) lpMemStr)
                    if (IsScalar (lpHeader->Rank))
                        cStrandNxtType = TranslateArrayTypeToStrandType (lpHeader->ArrType);
                    else
                        cStrandNxtType = STRAND_NESTED;
#undef  lpHeader
                    // We no longer need this ptr
                    MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbData)); lpMemStr = NULL;

                    break;
                } // End IF

                // Handle the immediate case

                // tkData is LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                // stData is an immediate
                Assert (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm);

                cStrandNxtType = TranslateImmTypeToStrandType (lpYYToken->tkToken.tkData.tkSym->stFlags.ImmType);

                break;

            case TKT_VARIMMED:
                cStrandNxtType = TranslateImmTypeToStrandType (lpYYToken->tkToken.tkFlags.ImmType);

                break;

            case TKT_CHRSTRAND:
                cStrandNxtType = STRAND_STRING;

                break;

            case TKT_NUMSTRAND:
                // Get the token attrs
                AttrsOfToken (&lpYYToken->tkToken, &aplTypeNum, &aplNELMNum, NULL, NULL);

                cStrandNxtType = TranslateArrayTypeToStrandType (aplTypeNum);

                // Count in the extra numeric length
                iLen += (UINT) aplNELMNum - 1;

                break;

            case TKT_VARARRAY:
                cStrandNxtType = STRAND_NESTED;

                break;

            case TKT_LISTPAR:
            case TKT_LSTIMMED:
            case TKT_LSTARRAY:
            case TKT_LSTMULT:
            case TKT_SYNTERR:           // Syntax Error
                goto SYNTAX_EXIT;

            case TKT_ASSIGN:
            case TKT_LISTSEP:
            case TKT_LABELSEP:
            case TKT_COLON:
            case TKT_FCNIMMED:
            case TKT_OP1IMMED:
            case TKT_OP2IMMED:
            case TKT_OP3IMMED:
            case TKT_OPJOTDOT:
            case TKT_LEFTPAREN:
            case TKT_RIGHTPAREN:
            case TKT_LEFTBRACKET:
            case TKT_RIGHTBRACKET:
            case TKT_LEFTBRACE:
            case TKT_RIGHTBRACE:
            case TKT_EOS:
            case TKT_EOL:
            case TKT_SOS:
            case TKT_LINECONT:
            case TKT_STRAND:
            case TKT_CS_ANDIF:          // Control structure:  ANDIF
            case TKT_CS_CASE:           // ...                 CASE
            case TKT_CS_CASELIST:       // ...                 CASELIST
            case TKT_CS_CONTINUE:       // ...                 CONTINUE
            case TKT_CS_ELSE:           // ...                 ELSE
            case TKT_CS_ELSEIF:         // ...                 ELSEIF
            case TKT_CS_END:            // ...                 END
            case TKT_CS_ENDFOR:         // ...                 ENDFOR
            case TKT_CS_ENDFORLCL:      // ...                 ENDFORLCL
            case TKT_CS_ENDIF:          // ...                 ENDIF
            case TKT_CS_ENDREPEAT:      // ...                 ENDREPEAT
            case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
            case TKT_CS_ENDWHILE:       // ...                 ENDWHILE
            case TKT_CS_FOR:            // ...                 FOR
            case TKT_CS_FOR2:           // ...                 FOR2
            case TKT_CS_FORLCL:         // ...                 FORLCL
            case TKT_CS_GOTO:           // ...                 GOTO
            case TKT_CS_IF:             // ...                 IF
            case TKT_CS_IF2:            // ...                 IF2
            case TKT_CS_IN:             // ...                 IN
            case TKT_CS_LEAVE:          // ...                 LEAVE
            case TKT_CS_ORIF:           // ...                 ORIF
            case TKT_CS_REPEAT:         // ...                 REPEAT
            case TKT_CS_REPEAT2:        // ...                 REPEAT2
            case TKT_CS_RETURN:         // ...                 RETURN
            case TKT_CS_SELECT:         // ...                 SELECT
            case TKT_CS_SELECT2:        // ...                 SELECT2
            case TKT_CS_UNTIL:          // ...                 UNTIL
            case TKT_CS_WHILE:          // ...                 WHILE
            case TKT_CS_WHILE2:         // ...                 WHILE2
            case TKT_CS_SKIPCASE:       // ...                 Special token
            case TKT_CS_SKIPEND:        // ...                 Special token
            case TKT_CS_NEC:            // ...                 Special token
            case TKT_CS_EOL:            // ...                 Special token
            defstop
                goto ERROR_EXIT;
        } // End SWITCH

        // Run through the conversion table
        cStrandCurType = tabConvert[cStrandCurType][cStrandNxtType];
    } // End FOR

    if (!bRet)
        goto ERROR_EXIT;

    aplTypeRes = TranslateStrandTypeToArrayType (cStrandCurType);

    //***********************************************************************
    //********** Single Element Case ****************************************
    //***********************************************************************

    // If there is a single element and the strand type is Boolean,
    //   Integer, Float, or Character (but not Charst), store it in
    //   a TKT_VARIMMED token, or if it's a string, pass on the existing
    //   HGLOBAL.
    if (IsSingleton (iNELM))
    {
        // Split cases based upon the strand's storage type
        switch (cStrandCurType)
        {
            case STRAND_BOOL:       // e.g., 0
            case STRAND_INT:        // e.g., 2
            case STRAND_CHAR:       // e.g., 'a'
            case STRAND_FLOAT:      // e.g., 1.5
            {
                // Split cases based upon the token flags
                switch (lpYYStrand->tkToken.tkFlags.TknType)
                {
                    case TKT_VARNAMED:
                        // tkData is an LPSYMENTRY
                        Assert (GetPtrTypeDir (lpYYStrand->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                        // Pass through the entire token
                        YYCopy (lpYYRes, lpYYStrand);

                        // If it's not an immediate, ...
                        if (!lpYYRes->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // stData is a NoValue entry or a valid HGLOBAL variable array
                            Assert (IsSymNoValue (lpYYRes->tkToken.tkData.tkSym)
                                 || IsGlbTypeVarDir (lpYYRes->tkToken.tkData.tkSym->stData.stGlbData));
                            // Make a copy of the token within if not a NoValue entry
                            if (!IsSymNoValue (lpYYRes->tkToken.tkData.tkSym))
                                lpYYRes->tkToken = *CopyToken_EM (&lpYYRes->tkToken, FALSE);
                        } // End IF

                        break;

                    case TKT_VARIMMED:
                        // Pass through the entire token
                        YYCopy (lpYYRes, lpYYStrand);

                        break;

                    case TKT_CHRSTRAND:
                    case TKT_NUMSTRAND:
                        // Fill in the result token
                        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                        lpYYRes->tkToken.tkData.tkGlbData  = lpYYStrand->tkToken.tkData.tkGlbData;
                        lpYYRes->tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

                        // Mark as reused
                        lpYYStrand->tkToken.tkData.tkGlbData = PTR_REUSED;

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;
            } // End STRAND_BOOL/INT/CHAR/FLOAT

            case STRAND_NESTED:     // e.g., a named variable that is not a simple scalar
                // Split cases based upon the token type
                switch (lpYYStrand->tkToken.tkFlags.TknType)
                {
                    case TKT_VARNAMED:
                        // tkData is an LPSYMENTRY
                        Assert (GetPtrTypeDir (lpYYStrand->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                        // It's not immediate (handled above)
                        Assert (!lpYYStrand->tkToken.tkData.tkSym->stFlags.Imm);

                        // stData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (lpYYStrand->tkToken.tkData.tkSym->stData.stGlbData));

                        // Pass through the entire token
                        YYCopy (lpYYRes, lpYYStrand);

                        // Make a copy of the token within
                        lpYYRes->tkToken = *CopyToken_EM (&lpYYStrand->tkToken, FALSE);

                        break;

                    case TKT_VARARRAY:
                        // tkData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (lpYYStrand->tkToken.tkData.tkGlbData));

                        // Fill in the result token
                        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                        lpYYRes->tkToken.tkData.tkGlbData  = lpYYStrand->tkToken.tkData.tkGlbData;
                        lpYYRes->tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

                        // Mark as reused
                        lpYYStrand->tkToken.tkData.tkGlbData = PTR_REUSED;

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            case STRAND_STRING:     // e.g., 'abc'
                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = lpYYStrand->tkToken.tkData.tkGlbData;
                lpYYRes->tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

                // Mark as reused
                lpYYStrand->tkToken.tkData.tkGlbData = PTR_REUSED;

                break;

            defstop
                break;
        } // End SWITCH

        goto NORMAL_EXIT;
    } // End IF

    //***********************************************************************
    //********** Multiple Element Case **************************************
    //***********************************************************************

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, iLen, 1);

    // Allocate global memory for a length <iLen> vector of type <aplTypeRes>.
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbStr = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbStr)
        goto WSFULL_EXIT;

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbStr);
    lpYYRes->tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

    // Lock the memory to get a ptr to it
    lpMemStr = MyGlobalLock (hGlbStr);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemStr)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType = aplTypeRes;
////lpHeader->PermNdx = PERMNDX_NONE;   // Already zero from GHND
////lpHeader->SysVar  = FALSE;          // Already zero from GHND
    lpHeader->RefCnt  = 1;
    lpHeader->NELM    = iLen;
    lpHeader->Rank    = 1;
#undef  lpHeader

    // Save the single dimension
    *VarArrayBaseToDim (lpMemStr) = iLen;

    // Skip over the header and one dimension (it's a vector)
    LPAPL.Bool = (LPAPLBOOL) VarArrayBaseToData (lpMemStr, 1);

    // Copy the elements to the global memory
    // Note we copy the elements in the reverse
    //   order they are on the stack because
    //   we parsed the tokenization from right
    //   to left.
    // The very fact that we have already determined
    //   the memory storage type, allows us to disregard
    //   possible conversion errors, such as if the final
    //   storage state is ARRAY_BOOL, that some token won't
    //   be Boolean.
    switch (cStrandCurType)
    {
        case STRAND_BOOL:           // lpYYToken->tkToken.tkData.tkBoolean (TKT_VARIMMED)
            // Initialize the bit index
            iBitIndex = 0;

            for (lpYYToken = &lpYYStrand[iNELM - 1];
                 lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            {
                // Split cases based upon the token type
                switch (lpYYToken->tkToken.tkFlags.TknType)
                {
                    case TKT_VARNAMED:
                        // tkData is an LPSYMENTRY
                        Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                        // If it's an immediate, ...
                        if (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // Copy the Boolean value to the result
                            *LPAPL.Bool |= (lpYYToken->tkToken.tkData.tkSym->stData.stBoolean) << iBitIndex++;
                        } else
                        {
                            // stData is an HGLOBAL
                            Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkSym->stData.stVoid) EQ PTRTYPE_HGLOBAL);

                            DbgStop ();         // ***FINISHME*** -- can we ever get here??
                                                //   Only if we allow a named var to point to
                                                //   a SYMENTRY which contains an HGLOBAL
                                                //   simple scalar.






                        } // End IF/ELSE

                        break;

                    case TKT_VARIMMED:
                        // Copy the Boolean value to the result
                        *LPAPL.Bool |= (lpYYToken->tkToken.tkData.tkBoolean) << iBitIndex++;

                        break;

                    case TKT_NUMSTRAND:
                        // Get and lock the global ptrs
                        GetGlbPtrs_LOCK (&lpYYToken->tkToken, &hGlbNum, &lpMemNum);

                        // Skip over the header and dimensions to the data
                        lpMemNum = VarArrayBaseToData (lpMemNum, 1);

                        // Clear the ptr type bits
                        hGlbNum = ClrPtrTypeDirAsGlb (hGlbNum);

                        // Loop through the numeric strand
                        for (uNum = 0; uNum < aplNELMNum; uNum++)
                        {
                            // Get the next value from memory
                            GetNextValueMem (lpMemNum,          // Ptr to item global memory data
                                             aplTypeNum,        // Item storage type
                                             uNum,              // Index into item
                                             NULL,              // Ptr to result global memory handle (may be NULL)
                                            &aplLongestNum,     // Ptr to result immediate value (may be NULL)
                                             NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                            // Copy the Boolean value to the result
                            *LPAPL.Bool |= aplLongestNum << iBitIndex++;

                            if (iBitIndex EQ NBIB)
                            {
                                iBitIndex = 0;      // Start over again
                                LPAPL.Bool++;       // Skip to next byte
                            } // End IF
                        } // End FOR

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbNum); lpMemNum = NULL;

                        break;

                    defstop
                        break;
                } // End SWITCH

                if (iBitIndex EQ NBIB)
                {
                    iBitIndex = 0;      // Start over again
                    LPAPL.Bool++;       // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case STRAND_INT:            // lpYYToken->tkToken.tkData.tkInteger (TKT_VARIMMED)
            for (lpYYToken = &lpYYStrand[iNELM - 1];
                 lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            // Split cases based upon the token type
            switch (lpYYToken->tkToken.tkFlags.TknType)
            {
                case TKT_VARNAMED:
                    // tkData is an LPSYMENTRY
                    Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                    // If it's an immediate, ...
                    if (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                        // Copy the integer value to the result
                        *LPAPL.Int++ = lpYYToken->tkToken.tkData.tkSym->stData.stInteger;
                    else
                    {
                        // stData is an HGLOBAL
                        Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkSym->stData.stVoid) EQ PTRTYPE_HGLOBAL);

                        DbgStop ();             // ***FINISHME*** -- can we ever get here??
                                                //   Only if we allow a named var to point to
                                                //   a SYMENTRY which contains an HGLOBAL
                                                //   simple scalar.






                    } // End IF/ELSE

                    break;

                case TKT_VARIMMED:
                    // Copy the integer value to the result
                    *LPAPL.Int++ = lpYYToken->tkToken.tkData.tkInteger;

                    break;

                case TKT_NUMSTRAND:
                    // Get the numeric strand attrs
                    AttrsOfToken (&lpYYToken->tkToken, &aplTypeNum, &aplNELMNum, NULL, NULL);

                    Assert (aplNELMNum > 0);

                    // Get and lock the global ptrs
                    GetGlbPtrs_LOCK (&lpYYToken->tkToken, &hGlbNum, &lpMemNum);

                    // Skip over the header and dimensions to the data
                    lpMemNum = VarArrayBaseToData (lpMemNum, 1);

                    // Clear the ptr type bits
                    hGlbNum = ClrPtrTypeDirAsGlb (hGlbNum);

                    // Loop through the numeric strand
                    for (uNum = 0; uNum < aplNELMNum; uNum++)
                    {
                        // Get the next value from memory
                        GetNextValueMem (lpMemNum,          // Ptr to item global memory data
                                         aplTypeNum,        // Item storage type
                                         uNum,              // Index into item
                                         NULL,              // Ptr to result global memory handle (may be NULL)
                                        &aplLongestNum,     // Ptr to result immediate value (may be NULL)
                                         NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        // Copy the integer value to the result
                        *LPAPL.Int++ = aplLongestNum;
                    } // End FOR

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbNum); lpMemNum = NULL;

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case STRAND_CHAR:
        case STRAND_CHARST:         // lpYYToken->tkToken.tkData.tkChar (TKT_VARIMMED)
            for (lpYYToken = &lpYYStrand[iNELM - 1];
                 lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            // Split cases based upon the token type
            switch (lpYYToken->tkToken.tkFlags.TknType)
            {
                case TKT_VARNAMED:
                    // tkData is an LPSYMENTRY
                    Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                    // If it's an immediate, ...
                    if (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                        // Copy the char value to the result
                        *LPAPL.Char++ = lpYYToken->tkToken.tkData.tkSym->stData.stChar;
                    else
                    {
                        // stData is an HGLOBAL
                        Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkSym->stData.stVoid) EQ PTRTYPE_HGLOBAL);

                        DbgStop ();             // ***FINISHME*** -- can we ever get here??
                                                //   Only if we allow a named var to point to
                                                //   a SYMENTRY which contains an HGLOBAL
                                                //   simple scalar.






                    } // End IF/ELSE

                    break;

                case TKT_VARIMMED:
                    // Copy the char value to the result
                    *LPAPL.Char++ = lpYYToken->tkToken.tkData.tkChar;

                    break;

                case TKT_NUMSTRAND:                         // Can't happen -- STRAND_CHAR
                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case STRAND_FLOAT:          // lpYYToken->tkToken.tkData.tkFloat (TKT_VARIMMED)
            for (lpYYToken = &lpYYStrand[iNELM - 1];
                 bRet && lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            // Split cases based upon the token type
            switch (lpYYToken->tkToken.tkFlags.TknType)
            {
                case TKT_VARNAMED:
                    // tkData is an LPSYMENTRY
                    Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                    // If it's an immediate, ...
                    if (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                    {
                        // Split cases based upon the symbol table entry's immediate type
                        switch (lpYYToken->tkToken.tkData.tkSym->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:
                                // Promote and copy the Boolean value to the result
                                *LPAPL.Float++ = (APLFLOAT) (lpYYToken->tkToken.tkData.tkSym->stData.stBoolean);

                                break;

                            case IMMTYPE_INT:
                                // ***FIXME*** -- Possible loss of precision

                                // Promote and copy the integer value to the result
                                *LPAPL.Float++ = (APLFLOAT) (lpYYToken->tkToken.tkData.tkSym->stData.stInteger);

                                break;

                            case IMMTYPE_FLOAT:
                                // Copy the float value to the result
                                *LPAPL.Float++ = lpYYToken->tkToken.tkData.tkSym->stData.stFloat;

                                break;

                            case IMMTYPE_CHAR:      // We should never get here
                            defstop
                                break;
                        } // End SWITCH
                    } else
                    {
                        DbgStop ();         // We should never get here



                    } // End IF/ELSE

                    break;

                case TKT_VARIMMED:
                    // Split cases based upon the token's immediate type
                    switch (lpYYToken->tkToken.tkFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                            // Promote and copy the Boolean value to the result
                            *LPAPL.Float++ = (APLFLOAT) (lpYYToken->tkToken.tkData.tkBoolean);

                            break;

                        case IMMTYPE_INT:
                            // Promote and copy the integer value to the result
                            *LPAPL.Float++ = (APLFLOAT) (lpYYToken->tkToken.tkData.tkInteger);

                            break;

                        case IMMTYPE_FLOAT:
                            // Copy the float value to the result
                            *LPAPL.Float++ = lpYYToken->tkToken.tkData.tkFloat;

                            break;

                        case IMMTYPE_CHAR:      // We should never get here
                        defstop
                            break;
                    } // End SWITCH

                    break;

                case TKT_NUMSTRAND:
                    // Get the numeric strand attrs
                    AttrsOfToken (&lpYYToken->tkToken, &aplTypeNum, &aplNELMNum, NULL, NULL);

                    Assert (aplNELMNum > 0);

                    // Get and lock the global ptrs
                    GetGlbPtrs_LOCK (&lpYYToken->tkToken, &hGlbNum, &lpMemNum);

                    // Skip over the header and dimensions to the data
                    lpMemNum = VarArrayBaseToData (lpMemNum, 1);

                    // Clear the ptr type bits
                    hGlbNum = ClrPtrTypeDirAsGlb (hGlbNum);

                    // Loop through the numeric strand
                    for (uNum = 0; uNum < aplNELMNum; uNum++)
                    {
                        // Get the next value from memory
                        GetNextValueMem (lpMemNum,          // Ptr to item global memory data
                                         aplTypeNum,        // Item storage type
                                         uNum,              // Index into item
                                         NULL,              // Ptr to result global memory handle (may be NULL)
                                        &aplLongestNum,     // Ptr to result immediate value (may be NULL)
                                         NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        // If the numeric strand is integer, ...
                        if (IsSimpleInt (aplTypeNum))
                            // Copy the integer value to the float result
                            *LPAPL.Float++ = (APLFLOAT) (APLINT) aplLongestNum;
                        else
                            // Copy the float value to the float result
                            *LPAPL.Float++ = *(LPAPLFLOAT) &aplLongestNum;
                    } // End FOR

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbNum); lpMemNum = NULL;

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case STRAND_HETERO:     // lpYYToken->tkToken.tkData.tkSym (LPSYMENTRY)
        case STRAND_NESTED:     // lpYYToken->tkToken.tkData.aplNested (LPSYMENTRY or HGLOBAL)
            for (lpYYToken = &lpYYStrand[iNELM - 1];
                 bRet && lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            switch (lpYYToken->tkToken.tkFlags.TknType)
            {
                case TKT_VARNAMED:    // 1 a
                    // tkData is an LPSYMENTRY
                    Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                    // If the STE is immediate, make a copy of it
                    if (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                    {
                        // Make a copy of the symbol table entry as we can't use the
                        //   one in the STNAME.
                        lpSymEntry = CopyImmSymEntry_EM (lpYYToken->tkToken.tkData.tkSym,
                                                         -1,
                                                        &lpYYToken->tkToken);
                        if (lpSymEntry)
                            // Save the symbol table entry and skip past it
                            *LPAPL.Sym++ = MakePtrTypeSym (lpSymEntry);
                        else
                            bRet = FALSE;
                    } else
                    {
                        // stData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.tkSym->stData.stGlbData));

                        *LPAPL.Nested++ = CopySymGlbDir (lpYYToken->tkToken.tkData.tkSym->stData.stGlbData);
                    } // End IF/ELSE

                    break;

                case TKT_VARIMMED:  // 1.5 'ab'
                    // Copy the immediate token as an LPSYMENTRY
                    lpSymEntry = CopyImmToken_EM (&lpYYToken->tkToken);
                    if (lpSymEntry)
                        *LPAPL.Nested++ = lpSymEntry;
                    else
                        bRet = FALSE;
                    break;

                case TKT_VARARRAY:  // 1('ab')
                case TKT_CHRSTRAND: // 1 'ab'
                    // tkData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.tkGlbData));

                    // Copy the nested entry to the result
                    *LPAPL.Nested++ = lpYYToken->tkToken.tkData.tkGlbData;

                    // Mark as reused
                    lpYYToken->tkToken.tkData.tkGlbData = PTR_REUSED;

                    break;

                case TKT_NUMSTRAND:
                    // Get the numeric strand attrs
                    AttrsOfToken (&lpYYToken->tkToken, &aplTypeNum, &aplNELMNum, NULL, NULL);

                    Assert (aplNELMNum > 0);

                    // Get and lock the global ptrs
                    GetGlbPtrs_LOCK (&lpYYToken->tkToken, &hGlbNum, &lpMemNum);

                    // Skip over the header and dimensions to the data
                    lpMemNum = VarArrayBaseToData (lpMemNum, 1);

                    // Get the numeric strand immediate type
                    immTypeNum = TranslateArrayTypeToImmType (aplTypeNum);

                    // Clear the ptr type bits
                    hGlbData = ClrPtrTypeDirAsGlb (hGlbData);

                    // Loop through the numeric strand
                    for (uNum = 0; uNum < aplNELMNum; uNum++)
                    {
                        // Get the next value from memory
                        GetNextValueMem (lpMemNum,          // Ptr to item global memory data
                                         aplTypeNum,        // Item storage type
                                         uNum,              // Index into item
                                         NULL,              // Ptr to result global memory handle (may be NULL)
                                        &aplLongestNum,     // Ptr to result immediate value (may be NULL)
                                         NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        // Make a SYMENTRY
                        lpSymEntry = MakeSymEntry_EM (immTypeNum,           // Immediate type
                                                     &aplLongestNum,        // Ptr to immediate value
                                                     &lpYYArg->tkToken);    // Ptr to function token
                        if (lpSymEntry)
                            *LPAPL.Nested++ = lpSymEntry;
                        else
                        {
                            bRet = FALSE;

                            break;
                        } // End IF/ELSE
                    } // End FOR

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbNum); lpMemNum = NULL;

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case STRAND_STRING:
        defstop
            break;
    } // End FOR/SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbStr); lpMemStr = NULL;

    if (!bRet)
        goto ERROR_EXIT;
NORMAL_EXIT:
    // Free the tokens on this portion of the strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[STRAND_VAR], lpplLocalVars->lpYYStrandBase[STRAND_VAR]);

    // Strip the tokens on this portion of the strand stack
    StripStrand (lpYYRes, STRAND_VAR);

    DBGLEAVE;

    return lpYYRes;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpYYToken->tkToken);
    goto ERROR_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYArg->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Free the entire strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[STRAND_VAR], lpplLocalVars->lpYYStrandStart[STRAND_VAR]);

    DBGLEAVE;

    YYFree (lpYYRes); lpYYRes = NULL; return NULL;
} // End MakeVarStrand_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeFcnStrand_EM_YY
//
//  Make the function strand into an immediate token or a global memory array.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeFcnStrand_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakeFcnStrand_EM_YY
    (LPPL_YYSTYPE lpYYArg,          // Ptr to incoming token
     NAME_TYPES   fnNameType,       // Type of the strand
     UBOOL        bSaveTxtLine)     // TRUE iff we should save the line text

{
    UINT          uIniLen,          // Initial strand length
                  uActLen,          // Actual  ...
                  uCnt,             // Loop counter
                  TknCount;         // Token count
    APLUINT       ByteRes;          // # bytes in the result
    HGLOBAL       hGlbStr;
    LPVOID        lpMemStr;
    LPPL_YYSTYPE  lpYYStrand,
                  lpYYMemStart,
                  lpYYMemData,
                  lpYYBase = (LPPL_YYSTYPE) -1,
                  lpYYRes;          // Ptr to the result
    UBOOL         bRet = TRUE;      // TRUE iff the result is valid
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars
    SYSTEMTIME    systemTime;       // Current system (UTC) time

    DBGENTER;

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    Assert (YYCheckInuse (lpYYRes));        // ***DEBUG***

    // Save the base of this strand
    lpYYStrand              =
    lpYYRes->lpYYStrandBase = lpYYArg->lpYYStrandBase;

    // Count the # tokens in the strand
    uIniLen = YYCountFcnStr (lpYYArg->lpYYFcnBase);

    //***********************************************************************
    //********** Single Element Case ****************************************
    //***********************************************************************

    // If there is a single element, pass through the entire token
    if (IsSingleton (uIniLen))
    {
        // Free the allocated result as CopyPL_YYSTYPE_EM_YY
        //   will allocate a result
        YYFree (lpYYRes); lpYYRes = NULL;

        // Copy the entire token
        lpYYRes = CopyPL_YYSTYPE_EM_YY (lpYYArg->lpYYFcnBase, FALSE);
        lpYYRes->TknCount = 1;

        lpYYBase = lpYYArg->lpYYFcnBase;
        lpYYRes->lpYYFcnBase = NULL;            // No longer valid

        Assert (YYCheckInuse (lpYYRes));        // ***DEBUG***

        goto NORMAL_EXIT;
    } // End IF

    //***********************************************************************
    //********** Multiple Element Case **************************************
    //***********************************************************************

    // In some cases, the following count (iLen) might be larger than needed
    //   because it may count some elements of the result twice because of
    //   calls to PushFcnStrand_YY on the same argument (perhaps once for PRIMFCN,
    //   and later on once for LeftFunc).  The overcount is harmless and ignored.

    // Calculate the # bytes we'll need for the header and data
    ByteRes = CalcFcnSize (uIniLen);

    // Allocate global memory for the function array
    // N.B.: Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbStr = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbStr)
        goto WSFULL_EXIT;

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_FCNARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbStr);
    lpYYRes->tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;

    Assert (YYCheckInuse (lpYYRes));        // ***DEBUG***

    // Lock the memory to get a ptr to it
    lpMemStr = MyGlobalLock (hGlbStr);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemStr)
    // Fill in the header
    lpHeader->Sig.nature  = FCNARRAY_HEADER_SIGNATURE;
    lpHeader->fnNameType  = fnNameType;
    lpHeader->RefCnt      = 1;
    lpHeader->tknNELM     = uIniLen;

    // Get the current system (UTC) time
    GetSystemTime (&systemTime);

    // Convert system time to file time and save as creation time
    SystemTimeToFileTime (&systemTime, &lpHeader->ftCreation);

    // Use the same time as the last modification time
    lpHeader->ftLastMod = lpHeader->ftCreation;

    Assert (YYCheckInuse (lpYYRes));        // ***DEBUG***

    // Skip over the header to the data (PL_YYSTYPEs)
    lpYYMemStart = lpYYMemData = FcnArrayBaseToData (lpMemStr);

    // Initialize token count
    TknCount = 0;

    // Copy the PL_YYSTYPEs to the global memory object
    lpYYMemData = YYCopyFcn (lpYYMemData, lpYYArg->lpYYFcnBase, &lpYYBase, &TknCount, TRUE);

    // Calculate the actual length
    lpHeader->tknNELM = uActLen = (UINT) (lpYYMemData - lpYYMemStart);

    Assert (TknCount EQ uIniLen);
    Assert (uActLen  EQ uIniLen);

    // Zap all occurrences of <lpYYFcnBase> as they are no longer valid
    for (uCnt = 0; uCnt < uActLen; uCnt++)
        lpYYMemStart[uCnt].lpYYFcnBase = NULL;

    if (bSaveTxtLine)
    {
        HGLOBAL        hGlbPTD;         // PerTabData global memory handle
        LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
        UINT           uLineLen;        // Line length
        LPMEMTXT_UNION lpMemTxtLine;    // Ptr to line text global memory
        LPAPLCHAR      lpMemTxtSrc,     // Ptr to start of WCHARs
                       lpMemTxtEnd;     // ...    end   ...

        // Get the PerTabData global memory handle
        hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Use a temp var
        lpMemTxtSrc = lpMemPTD->lpwszTemp;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

        // Convert the tokens to WCHARs
        lpMemTxtEnd =
          DisplayFcnGlb (lpMemTxtSrc,       // Ptr to output save area
                         hGlbStr,           // Function array global memory handle
                         FALSE,             // TRUE iff we're to display the header
                         NULL,              // Ptr to function to convert an HGLOBAL to FMTSTR_GLBOBJ (may be NULL)
                         NULL);             // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
        // Get the line length
        uLineLen = (UINT) (lpMemTxtEnd - lpMemTxtSrc);

        // Allocate global memory for a length <uLineLen> vector of type <APLCHAR>.
        lpHeader->hGlbTxtLine = DbgGlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1) * sizeof (lpMemTxtLine->C));
        if (lpHeader->hGlbTxtLine)
        {
            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (lpHeader->hGlbTxtLine);

            // Save the line length
            lpMemTxtLine->U = uLineLen;

            // Copy the line text to global memory
            CopyMemory (&lpMemTxtLine->C, lpMemTxtSrc, uLineLen * sizeof (lpMemTxtLine->C));

            // We no longer need this ptr
            MyGlobalUnlock (lpHeader->hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbStr); lpMemStr = lpYYMemData = lpYYMemStart = NULL;
#undef  lpHeader

    Assert (YYCheckInuse (lpYYRes));        // ***DEBUG***

    Assert (TknCount EQ uActLen);

    // Save the token & function counts
    lpYYRes->TknCount = TknCount;
#ifdef DEBUG
    if (hGlbStr)
        // Display the function array
        DisplayFcnArr (hGlbStr);
#endif
NORMAL_EXIT:
    lpYYRes->lpYYStrandBase  = lpplLocalVars->lpYYStrandBase[STRAND_FCN] = lpYYBase;

    Assert (YYCheckInuse (lpYYRes));        // ***DEBUG***

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (STRAND_FCN);
#endif

    // Free the tokens on this portion of the strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[STRAND_FCN], lpplLocalVars->lpYYStrandBase[STRAND_FCN]);

    // Strip the tokens on this portion of the strand stack
    StripStrand (lpYYRes, STRAND_FCN);

    DBGLEAVE;

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Free the entire function strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[STRAND_FCN], lpplLocalVars->lpYYStrandStart[STRAND_FCN]);

    DBGLEAVE;

    YYFree (lpYYRes); lpYYRes = NULL; return NULL;
} // End MakeFcnStrand_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $CopyString_EM_YY
//
//  Copy a numeric or character string
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyString_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE CopyString_EM_YY
    (LPPL_YYSTYPE lpYYStr)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (lpYYStr->tkToken.tkData.tkGlbData));

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = lpYYStr->tkToken.tkFlags.TknType;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir (lpYYStr->tkToken.tkData.tkGlbData);
    lpYYRes->tkToken.tkCharIndex       = lpYYStr->tkToken.tkCharIndex;

    DBGLEAVE;

    return lpYYRes;
} // End CopyString_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeAxis_YY
//
//  Make an axis value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeAxis_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakeAxis_YY
    (LPPL_YYSTYPE lpYYAxis)     // Ptr to axis value

{
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Split cases based upon the token type
    switch (lpYYAxis->tkToken.tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYAxis->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate, ...
            if (!lpYYAxis->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lpYYAxis->tkToken.tkData.tkSym->stData.stGlbData));

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_AXISARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir (lpYYAxis->tkToken.tkData.tkSym->stData.stGlbData);
                lpYYRes->tkToken.tkCharIndex       = lpYYAxis->tkToken.tkCharIndex;

                break;
            } // End IF

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_AXISIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = lpYYAxis->tkToken.tkData.tkSym->stFlags.ImmType;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkLongest  = lpYYAxis->tkToken.tkData.tkSym->stData.stLongest;
            lpYYRes->tkToken.tkCharIndex       = lpYYAxis->tkToken.tkCharIndex;

            break;

        case TKT_VARIMMED:
            // Copy the token and rename it
            YYCopy (lpYYRes, lpYYAxis);     // No need to CopyPL_YYSTYPE_EM_YY immediates
            lpYYRes->tkToken.tkFlags.TknType   = TKT_AXISIMMED;

            break;

        case TKT_VARARRAY:
            // Free the result as CopyPL_YYSTYPE_EM_YY
            //   will allocate a result
            YYFree (lpYYRes); lpYYRes = NULL;

            // Copy the token and rename it
            lpYYRes = CopyPL_YYSTYPE_EM_YY (lpYYAxis, FALSE);
            lpYYRes->tkToken.tkFlags.TknType = TKT_AXISARRAY;

            break;

        defstop
            break;
    } // End SWITCH

    DBGLEAVE;

    return lpYYRes;
} // End MakeAxis_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakePrimFcn_YY
//
//  Make a token a primitive function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePrimFcn_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakePrimFcn_YY
    (LPPL_YYSTYPE lpYYFcn)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    DBGENTER;

    lpYYRes = CopyPL_YYSTYPE_EM_YY (lpYYFcn, FALSE);
    lpYYRes->tkToken.tkFlags.TknType = TKT_FCNIMMED;
    lpYYRes->tkToken.tkFlags.ImmType = GetImmTypeFcn (lpYYFcn->tkToken.tkData.tkChar);
    lpYYRes->lpYYFcnBase = NULL;
    lpYYRes->TknCount = 1;

    DBGLEAVE;

    return lpYYRes;
} // End MakePrimFcn_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeNameFcnOpr_YY
//
//  Make a token for a named function, monadic/dyadic/ambiguous operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeNameFcnOpr_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakeNameFcnOpr_YY
    (LPPL_YYSTYPE lpYYFcnOpr)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    lpYYRes = CopyPL_YYSTYPE_EM_YY (lpYYFcnOpr, FALSE);
    lpYYRes->lpYYFcnBase = NULL;
    lpYYRes->TknCount    =
    lpYYRes->Avail       = 0;
#ifdef DEBUG
    lpYYRes->YYIndex     = NEG1U;
    lpYYRes->YYFlag      = 0;
#endif
    return lpYYRes;
} // End MakeNameFcnOpr_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakePrimOp1_YY
//
//  Make a primitive monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePrimOp1_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakePrimOp1_YY
    (LPPL_YYSTYPE lpYYOp1)

{
    return MakePrimOp123_YY (lpYYOp1, IMMTYPE_PRIMOP1);
} // End MakePrimOp1_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakePrimOp2_YY
//
//  Make a primitive dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePrimOp2_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakePrimOp2_YY
    (LPPL_YYSTYPE lpYYOp2)

{
    return MakePrimOp123_YY (lpYYOp2, IMMTYPE_PRIMOP2);
} // End MakePrimOp2_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakePrimOp3_YY
//
//  Make a primitive ambiguous operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePrimOp3_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakePrimOp3_YY
    (LPPL_YYSTYPE lpYYOp3)

{
    return MakePrimOp123_YY (lpYYOp3, IMMTYPE_PRIMOP3);
} // End MakePrimOp3_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakePrimOp123_YY
//
//  Common routine to MakePrimOp?_YY
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePrimOp123_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakePrimOp123_YY
    (LPPL_YYSTYPE lpYYOp123,
     IMM_TYPES    immType)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    lpYYRes = CopyPL_YYSTYPE_EM_YY (lpYYOp123, FALSE);
    lpYYRes->lpYYFcnBase = NULL;
    lpYYRes->TknCount    =
    lpYYRes->Avail       = 0;
#ifdef DEBUG
    lpYYRes->YYIndex     = NEG1U;
    lpYYRes->YYFlag      = 0;
#endif
    return lpYYRes;
} // End MakePrimOp123_YY
#undef  APPEND_NAME


//***************************************************************************
//  $InitNameStrand
//
//  Initialize a name strand
//***************************************************************************

void InitNameStrand
    (LPPL_YYSTYPE lpYYArg)          // Ptr to the incoming argument

{
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Set the base of this strand to the next available location
    lpYYArg->lpYYStrandBase                   =
    lpplLocalVars->lpYYStrandBase[STRAND_VAR] = lpplLocalVars->lpYYStrandNext[STRAND_VAR];
} // End InitNameStrand


//***************************************************************************
//  $PushNameStrand_YY
//
//  Push a name strand
//***************************************************************************

LPPL_YYSTYPE PushNameStrand_YY
    (LPPL_YYSTYPE lpYYArg)          // Ptr to the incoming argument

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_STRAND;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = NEG1U; // Debug value
    lpYYRes->tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;

    lpYYRes->lpYYStrandBase  =
    lpYYArg->lpYYStrandBase  = lpplLocalVars->lpYYStrandBase[STRAND_VAR];

    // Save this token on the strand stack
    //   and skip over it
    YYCopyFreeDst (lpplLocalVars->lpYYStrandNext[STRAND_VAR]++, lpYYArg);

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (STRAND_VAR);
#endif
    return lpYYRes;
} // End PushNameStrand_YY


//***************************************************************************
//  $MakeNameStrand_EM_YY
//
//  Make a name strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeNameStrand_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakeNameStrand_EM_YY
    (LPPL_YYSTYPE lpYYArg)          // Ptr to incoming token

{
    int           iLen;             // # elements in the strand
    APLUINT       ByteRes;          // # bytes in the result
    LPPL_YYSTYPE  lpYYStrand;       // Ptr to base of strand
    HGLOBAL       hGlbStr;          // Strand global memory handle
    LPVOID        lpMemStr;         // Ptr to strand global memory
    LPPL_YYSTYPE  lpYYRes;          // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Save the base of this strand
    lpYYStrand              =
    lpYYRes->lpYYStrandBase = lpYYArg->lpYYStrandBase;

    // Get the # elements in the strand
    iLen = (UINT) (lpplLocalVars->lpYYStrandNext[STRAND_VAR] - lpYYStrand);

    // Save these tokens in global memory

    // Calculate storage needed for the tokens
    ByteRes = sizeof (VARNAMED_HEADER)          // For the header
            + sizeof (lpYYStrand[0]) * iLen;    // For the data

    // Allocate global memory for a length <iLen> vector of type <cState>
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbStr = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbStr)
        goto WSFULL_EXIT;

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_STRNAMED;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbStr);
    lpYYRes->tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

    // Lock the memory to get a ptr to it
    lpMemStr = MyGlobalLock (hGlbStr);

#define lpHeader    ((LPVARNAMED_HEADER) lpMemStr)
    // Fill in the header
    lpHeader->Sig.nature = VARNAMED_HEADER_SIGNATURE;
    lpHeader->NELM       = iLen;
#undef  lpHeader

    // Skip over the header to the data
    lpMemStr = VarNamedBaseToData (lpMemStr);

    // Copy the tokens to global memory
    CopyMemory (lpMemStr, lpYYStrand, iLen * sizeof (lpYYStrand[0]));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbStr); lpMemStr = NULL;

    // Free the tokens on this portion of the strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[STRAND_VAR], lpplLocalVars->lpYYStrandBase[STRAND_VAR]);

    // Strip the tokens on this portion of the strand stack
    StripStrand (lpYYRes, STRAND_VAR);

    DBGLEAVE;

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Free the entire strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[STRAND_VAR], lpplLocalVars->lpYYStrandStart[STRAND_VAR]);

    DBGLEAVE;

    YYFree (lpYYRes); lpYYRes = NULL; return NULL;
} // End MakeNameStrand_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $InitList0_YY
//
//  Initialize a list starting with an empty token
//***************************************************************************

LPPL_YYSTYPE InitList0_YY
    (void)

{
    LPPL_YYSTYPE  lpYYRes;          // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_LISTINT;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = NEG1U;         // Debug value
    lpYYRes->tkToken.tkCharIndex       = NEG1U;

    // Set the base of this strand to the next available location
    lpYYRes->lpYYStrandBase                   =
    lpplLocalVars->lpYYStrandBase[STRAND_VAR] = lpplLocalVars->lpYYStrandNext[STRAND_VAR];

    return lpYYRes;
} // End InitList0_YY


//***************************************************************************
//  $InitList1_YY
//
//  Initialize a list starting with a single token
//***************************************************************************

LPPL_YYSTYPE InitList1_YY
    (LPPL_YYSTYPE lpYYArg)          // Ptr to incoming token

{
    LPPL_YYSTYPE  lpYYRes,          // Ptr to the result
                  lpYYLst;          // Ptr to the list

    // Initialize the list
    lpYYRes = InitList0_YY ();

    // Push an item onto the list
    lpYYLst = PushList_YY (lpYYRes, lpYYArg);
    FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;

    return lpYYLst;
} // End InitList1_YY


//***************************************************************************
//  $PushList_YY
//
//  Push a token onto the list stack
//***************************************************************************

LPPL_YYSTYPE PushList_YY
    (LPPL_YYSTYPE lpYYStrand,       // Ptr to base of strand
     LPPL_YYSTYPE lpYYArg)          // Ptr to incoming token (may be NULL)

{
    LPPL_YYSTYPE  lpYYRes;          // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars
    PL_YYSTYPE    YYTmp = {0};      // Temporary PL_YYSTYPE

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    YYCopy (lpYYRes, lpYYStrand);

    // If the token is NULL, push an empty token
    if (lpYYArg EQ NULL)
    {
        YYTmp.tkToken.tkFlags.TknType   = TKT_LISTSEP;
////////YYTmp.tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;    // Already zero from = {0}
////////YYTmp.tkToken.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
        YYTmp.tkToken.tkData.tkLongest  = NEG1U;            // Debug value
        YYTmp.tkToken.tkCharIndex       = NEG1U;            // ...
////////YYTmp.TknCount                  = 0;    // Already zero from = {0}
////////YYTmp.YYInuse                   = FALSE;// Already zero from = {0}
////////YYTmp.Indirect                  = FALSE;// Already zero from = {0}
////////YYTmp.Avail                     = 0;    // Already zero from = {0}
////////YYTmp.YYIndex                   = 0;    // Already zero from = {0}
////////YYTmp.YYFlag                    = 0;    // Already zero from = {0}
////////YYTmp.lpYYFcnBase               = NULL; // Already zero from = {0]
        YYTmp.lpYYStrandBase            = lpplLocalVars->lpYYStrandBase[STRAND_VAR];
        lpYYArg = &YYTmp;
    } // End IF

    // Copy this token to the strand stack
    //   and skip over it
    YYCopyFreeDst (lpplLocalVars->lpYYStrandNext[STRAND_VAR]++, lpYYArg);
#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (STRAND_VAR);
#endif
    return lpYYRes;
} // End PushList_YY


//***************************************************************************
//  $MakeList_EM_YY
//
//  Make the list into a global memory array.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeList_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE MakeList_EM_YY
    (LPPL_YYSTYPE lpYYArg,          // Ptr to incoming token
     UBOOL        bBrackets)        // TRUE iff surrounding brackets (otherwise parens)

{
    LPPL_YYSTYPE  lpYYStrand,       // Ptr to strand base
                  lpYYToken;        // Ptr to current strand token
    int           iLen;             // # items in the list
    HGLOBAL       hGlbLst = NULL;   // List global memory handle
    APLUINT       ByteRes;          // # bytes in the result
    LPAPLLIST     lpMemLst = NULL;  // Ptr to list global memory
    LPSYMENTRY    lpSymEntry;       // Ptr to current SYMENTRY
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    DBGENTER;

    // The list needs to be saved to global memory

    // Save the base of this strand
    lpYYStrand = lpYYArg->lpYYStrandBase;

    // Get the # elements in the strand
    iLen = (UINT) (lpplLocalVars->lpYYStrandNext[STRAND_VAR] - lpYYStrand);

    // If it's a single element in brackets (no semicolons), ...
    if (iLen EQ 1 && bBrackets)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken = lpYYStrand->tkToken;

        // Split cases based upon the token type
        switch (lpYYRes->tkToken.tkFlags.TknType)
        {
            case TKT_VARIMMED:
                // Set the new token type
                lpYYRes->tkToken.tkFlags.TknType = TKT_LSTIMMED;

                break;

            case TKT_VARARRAY:
                // Set the new token type
                lpYYRes->tkToken.tkFlags.TknType = TKT_LSTARRAY;
                DbgIncrRefCntDir (lpYYRes->tkToken.tkData.tkGlbData);

                break;

            case TKT_VARNAMED:
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYRes->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                // If it's not immediate, we must look inside the array
                if (!lpYYRes->tkToken.tkData.tkSym->stFlags.Imm)
                {
                    // Fill in the result token
                    lpYYRes->tkToken.tkFlags.TknType   = TKT_LSTARRAY;
////////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Set by lpYYRes->tkToken =
////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Set by lpYYRes->tkToken =
////                lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir (lpYYRes->tkToken.tkData.tkSym->stData.stGlbData);   // #1A
                    lpYYRes->tkToken.tkData.tkGlbData  =                lpYYRes->tkToken.tkData.tkSym->stData.stGlbData;    // #1B
////////////////////lpYYRes->tkToken.tkCharIndex       =                // Set by lpYYRes->tkToken =
                } else
                {
                    // Handle the immediate case

                    // Fill in the result token
                    lpYYRes->tkToken.tkFlags.TknType   = TKT_LSTIMMED;
                    lpYYRes->tkToken.tkFlags.ImmType   = lpYYRes->tkToken.tkData.tkSym->stFlags.ImmType;
////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Set by lpYYRes->tkToken =
                    lpYYRes->tkToken.tkData.tkLongest  = lpYYRes->tkToken.tkData.tkSym->stData.stLongest;
////////////////////lpYYRes->tkToken.tkCharIndex       =                // Set by lpYYRes->tkToken =
                } // End IF/ELSE

                break;

            defstop
                break;
        } // End SWITCH

        goto SAVEBASE_EXIT;
    } // End IF

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_LIST, iLen, 1);

    // Allocate global memory for a length <iLen> vector
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbLst = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbLst)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemLst = MyGlobalLock (hGlbLst);

#define lpHeader    ((LPLSTARRAY_HEADER) lpMemLst)
    // Fill in the header
    lpHeader->Sig.nature = LSTARRAY_HEADER_SIGNATURE;
    lpHeader->NELM       = iLen;
#undef  lpHeader

    // Skip over the header to the data
    lpMemLst = LstArrayBaseToData (lpMemLst);

    // Copy the elements to the global memory
    // Note we copy the elements in the reverse
    //   order they are on the stack because
    //   we parsed the tokenization from right
    //   to left.
    for (lpYYToken = &lpYYStrand[iLen - 1];
         lpYYToken NE &lpYYStrand[-1];
         lpYYToken--, lpMemLst++)
    // Split cases based upon the token type
    switch (lpYYToken->tkToken.tkFlags.TknType)
    {
        case TKT_VARNAMED:  // l[r]
            lpSymEntry = lpYYToken->tkToken.tkData.tkSym;

            if (lpSymEntry->stFlags.Imm)
            {
                // Fill in the result token
                lpMemLst->tkFlags.TknType   = TKT_VARIMMED;
                lpMemLst->tkFlags.ImmType   = lpSymEntry->stFlags.ImmType;
////////////////lpMemLst->tkFlags.NoDisplay = FALSE;    // Already zero from GHND
                lpMemLst->tkData.tkLongest  = lpSymEntry->stData.stLongest;
                lpMemLst->tkCharIndex       = lpYYToken->tkToken.tkCharIndex;
            } else
            {
                // Fill in the result token
                lpMemLst->tkFlags.TknType   = TKT_VARARRAY;
////////////////lpMemLst->tkFlags.ImmType   = IMMTYPE_ERROR;    // Already zero from GHND
////////////////lpMemLst->tkFlags.NoDisplay = FALSE;            // Already zero from GHND
////            lpMemLst->tkData.tkGlbData  = CopySymGlbDir (lpSymEntry->stData.stGlbData);     // #2A
                lpMemLst->tkData.tkGlbData  =                lpSymEntry->stData.stGlbData;      // #2B
                lpMemLst->tkCharIndex       = lpYYToken->tkToken.tkCharIndex;
            } // End IF/ELSE

            break;

        case TKT_VARIMMED:
        case TKT_LISTSEP:
            *lpMemLst = lpYYToken->tkToken;

            break;

        case TKT_VARARRAY:  // 1('ab')
        case TKT_CHRSTRAND: // 1 'ab'
            // Fill in the result token
            lpMemLst->tkFlags.TknType   = TKT_VARARRAY;
////////////lpMemLst->tkFlags.ImmType   = IMMTYPE_ERROR;        // Already zero from GHND
////////////lpMemLst->tkFlags.NoDisplay = FALSE;                // Already zero from GHND
            lpMemLst->tkData.tkGlbData  = CopySymGlbDir (lpYYToken->tkToken.tkData.tkGlbData);  // #3A
////        lpMemLst->tkData.tkGlbData  =                lpYYToken->tkToken.tkData.tkGlbData;   // #3B
            lpMemLst->tkCharIndex       = lpYYToken->tkToken.tkCharIndex;

            break;

        defstop
            break;
    } // End FOR/SWITCH

    // Unlock the handle
    MyGlobalUnlock (hGlbLst); lpMemLst = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = bBrackets ? TKT_LSTMULT : TKT_LISTPAR;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbLst);
    lpYYRes->tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;
SAVEBASE_EXIT:
    // Save the base of this strand
    lpYYRes->lpYYStrandBase = lpYYStrand;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbLst)
    {
        if (lpMemLst)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbLst); lpMemLst = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalLst (hGlbLst); hGlbLst = NULL;
    } // End IF
NORMAL_EXIT:
    // Free the tokens on this portion of the strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[STRAND_VAR], lpplLocalVars->lpYYStrandBase[STRAND_VAR]);

    // Strip from the strand stack
    StripStrand (lpYYRes, STRAND_VAR);

    DBGLEAVE;

    return lpYYRes;
} // End MakeList_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $CopyImmToken_EM
//
//  Copy an immediate token as a LPSYMENTRY
//***************************************************************************

LPSYMENTRY CopyImmToken_EM
    (LPTOKEN lpToken)

{
    LPSYMENTRY lpSymEntry;

    // Split cases based upon the token's immediate type
    switch (lpToken->tkFlags.ImmType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
            lpSymEntry = SymTabAppendInteger_EM (lpToken->tkData.tkInteger);

            break;

        case IMMTYPE_FLOAT:
            lpSymEntry = SymTabAppendFloat_EM   (lpToken->tkData.tkFloat);

            break;

        case IMMTYPE_CHAR:
            lpSymEntry = SymTabAppendChar_EM    (lpToken->tkData.tkChar);

            break;

        defstop
            return NULL;
    } // End SWITCH

    // If it failed, set the error token
    if (!lpSymEntry)
        ErrorMessageSetToken (lpToken);

    return lpSymEntry;
} // End CopyImmToken_EM


//***************************************************************************
//  $CopyToken_EM
//
//  Make a copy of a token, incrementing ref count if not changing
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyToken_EM"
#else
#define APPEND_NAME
#endif

LPTOKEN CopyToken_EM
    (LPTOKEN lpToken,
     UBOOL   bChanging) // TRUE iff we're going to change the HGLOBAL

{
    LPSYMENTRY lpSymEntry;  // Ptr to SYMENTRY in the token

    DBGENTER;

    // We haven't defined an instance of TRUE as yet
    Assert (bChanging EQ FALSE);

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:      // tkData is LPSYMENTRY
            // Get the LPSYMENTRY
            lpSymEntry = lpToken->tkData.tkSym;

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpSymEntry->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lpSymEntry->stData.stGlbData));

                // Increment the reference count in global memory
                DbgIncrRefCntDir (lpSymEntry->stData.stGlbData);

                break;          // We're done
            } // End IF

            break;              // Ignore immediates

        case TKT_FCNNAMED:      // tkData is LPSYMENTRY
        case TKT_OP1NAMED:      // tkData is LPSYMENTRY
        case TKT_OP2NAMED:      // tkData is LPSYMENTRY
        case TKT_OP3NAMED:      // tkData is LPSYMENTRY
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid direct function/operator
                //   or HGLOBAL function array
                //   or user-defined function/operator
                Assert (lpToken->tkData.tkSym->stFlags.FcnDir
                     || IsGlbTypeFcnDir (lpToken->tkData.tkSym->stData.stGlbData)
                     || IsGlbTypeDfnDir (lpToken->tkData.tkSym->stData.stGlbData));

                // If it's not a direct function/operator, ...
                if (!lpToken->tkData.tkSym->stFlags.FcnDir)
                    // Increment the reference count in global memory
                    DbgIncrRefCntDir (lpToken->tkData.tkSym->stData.stGlbData);

                break;          // We're done
            } // End IF

            break;              // Ignore immediates

        case TKT_VARARRAY:      // tkData is HGLOBAL
        case TKT_AXISARRAY:     // tkData is HGLOBAL
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lpToken->tkData.tkGlbData));

            // Increment the reference count in global memory
            DbgIncrRefCntDir (lpToken->tkData.tkGlbData);

            break;

        case TKT_FCNARRAY:      // tkData is HGLOBAL
            // tkData is a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir (lpToken->tkData.tkGlbData));

            // Increment the reference count in global memory
            DbgIncrRefCntDir (lpToken->tkData.tkGlbData);

            break;

        case TKT_VARIMMED:      // tkData is immediate
        case TKT_FCNIMMED:      // ...
        case TKT_AXISIMMED:     // ...
        case TKT_OP1IMMED:      // ...
        case TKT_OP2IMMED:      // ...
        case TKT_OP3IMMED:      // ...
        case TKT_OPJOTDOT:      // ...
            break;              // Ignore immediates

        case TKT_LISTPAR:       // tkData is HGLOBAL
        case TKT_LSTIMMED:      // tkData is immediate
        case TKT_LSTARRAY:      // tkData is HGLOBAL
        case TKT_LSTMULT:       // tkData is HGLOBAL
        defstop
            break;
    } // End SWITCH

    DBGLEAVE;

    return lpToken;
} // End CopyToken_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CopyPL_YYSTYPE_EM_YY
//
//  Make a copy of a PL_YYSTYPE, incrementing ref count if not changing
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyPL_YYSTYPE_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE CopyPL_YYSTYPE_EM_YY
    (LPPL_YYSTYPE lpYYArg,
     UBOOL        bChanging)    // TRUE iff we're going to change the HGLOBAL

{
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    Assert (bChanging EQ FALSE);

    // Copy the PL_YYSTYPE
    YYCopy (lpYYRes, lpYYArg);

    // Make a copy of the token within
    lpYYRes->tkToken = *CopyToken_EM (&lpYYArg->tkToken, bChanging);

    DBGLEAVE;

    return lpYYRes;
} // End CopyPL_YYSTYPE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: strand.c
//***************************************************************************
