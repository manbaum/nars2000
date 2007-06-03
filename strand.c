//***************************************************************************
//  NARS2000 -- Strand Routines
//***************************************************************************

#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

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
    (LPYYSTYPE lpYYArg)             // Ptr to the incoming argument

{
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Set the base of this strand to the next available location
    lpYYArg->unYYSTYPE.lpYYStrandBase        =
    lpplLocalVars->lpYYStrandBase[VARSTRAND] = lpplLocalVars->lpYYStrandNext[VARSTRAND];
} // End InitVarStrand


//***************************************************************************
//  $PushVarStrand_YY
//
//  Push a variable token onto the strand stack.
//***************************************************************************

LPYYSTYPE PushVarStrand_YY
    (LPYYSTYPE lpYYArg)             // Ptr to the incoming argument

{
    LPYYSTYPE     lpYYRes;
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_STRAND;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.lpVoid     = (LPVOID) -1;
    lpYYRes->tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;
    lpYYRes->TknCount                  =
    lpYYRes->FcnCount                  = 0;
    lpYYRes->lpYYFcn                   = NULL;

    lpYYRes->unYYSTYPE.lpYYStrandBase  =
    lpYYArg->unYYSTYPE.lpYYStrandBase  = lpplLocalVars->lpYYStrandBase[VARSTRAND];

    // Save this token on the strand stack
    //   and skip over it
    YYCopyFreeDst (lpplLocalVars->lpYYStrandNext[VARSTRAND]++, lpYYArg);

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (VARSTRAND);
#endif
    return lpYYRes;
} // End PushVarStrand_YY


//***************************************************************************
//  $PushFcnStrand_YY
//
//  Push a function token onto the strand stack.
//***************************************************************************

LPYYSTYPE PushFcnStrand_YY
    (LPYYSTYPE lpYYArg,             // Ptr to the incoming argument
     int       TknCount,            // Token count
     BOOL      bIndirect)           // TRUE iff lpYYArg is indirect

{
    LPYYSTYPE     lpYYRes,          // Ptr to result
                  lpYYCopy;         // Ptr to local copy
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    lpYYArg->TknCount = TknCount;
    lpYYArg->Indirect = bIndirect;
    lpYYArg->unYYSTYPE.lpYYStrandBase = lpplLocalVars->lpYYStrandBase[FCNSTRAND];
    if (!lpYYArg->lpYYFcn)
        lpYYArg->lpYYFcn = lpplLocalVars->lpYYStrandNext[FCNSTRAND];

    // Fill in the result token
    YYCopy (lpYYRes, lpYYArg);

    // Return our own position so the next user
    //   of this token can refer to it.
    lpYYRes->lpYYFcn = lpplLocalVars->lpYYStrandNext[FCNSTRAND];

    // Save this token on the strand stack
    //   and skip over it
    lpYYCopy = CopyYYSTYPE_EM_YY (lpYYArg, FALSE);
    YYCopyFreeDst (lpplLocalVars->lpYYStrandNext[FCNSTRAND]++, lpYYCopy);
    YYFree (lpYYCopy); lpYYCopy = NULL;

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (FCNSTRAND);
#endif
    return lpYYRes;
} // End PushFcnStrand_YY


//***************************************************************************
//  $StripStrand
//
//  Strip a strand from the strand stack
//***************************************************************************

void StripStrand
    (LPYYSTYPE lpYYStrand,          // Ptr to base of strand to strip
     int       strType)             // Strand type (VARSTRAND or FCNSTRAND)

{
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // If we're not back at the beginning, set the new base
    //   to the base of the token previous to the current base
    if (lpplLocalVars->lpYYStrandBase[strType] NE lpplLocalVars->lpYYStrandStart[strType])
        lpplLocalVars->lpYYStrandBase[strType] = lpplLocalVars->lpYYStrandBase[strType][-1].unYYSTYPE.lpYYStrandBase;

    // Set next available slot to this YYtoken's base
    lpplLocalVars->lpYYStrandNext[strType] = lpYYStrand->unYYSTYPE.lpYYStrandBase;

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
    (LPYYSTYPE lpYYStrandNext,      // Ptr to next strand element
     LPYYSTYPE lpYYStrand)          // Ptr to strand base

{
    int       iLen;
    LPYYSTYPE lpYYToken;

    // Get the # elements in the strand
    iLen = lpYYStrandNext - lpYYStrand;

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
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                // Does it have a value?
                if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Value)
                    break;

                if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                {
                    // stData is a valid HGLOBAL variable or function array
                    Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.tkSym->stData.stGlbData)
                         || IsGlbTypeFcnDir (lpYYToken->tkToken.tkData.tkSym->stData.stGlbData));
                } // End IF

                break;          // Don't free names

            case TKT_STRING:
            case TKT_VARARRAY:
            case TKT_AXISARRAY:
                // Check for reused ptrs
                if (!PtrReusedDir (lpYYToken->tkToken.tkData.tkGlbData))
                {
                    // tkData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.tkGlbData));

                    if (FreeResultGlobalVar (ClrPtrTypeDirGlb (lpYYToken->tkToken.tkData.tkGlbData)))
                    {
#ifdef DEBUG
                        dprintf ("**Zapping in FreeStrand: %08X (%s#%d)",
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
            case TKT_OPJOTDOT:
            case TKT_LPAREN:
            case TKT_RPAREN:
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

LPYYSTYPE MakeVarStrand_EM_YY
    (LPYYSTYPE lpYYArg)             // Ptr to incoming token

{
    int         iLen,
                iBitIndex;
    APLUINT     ByteRes;                // # bytes needed for the result
    LPYYSTYPE   lpYYToken,
                lpYYStrand;
    HGLOBAL     hGlbStr,
                hGlbData;
    LPVOID      lpMemStr;
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
                  cStrandNxtType,
                  aplType;
    BOOL          bRet = TRUE;
    LPYYSTYPE     lpYYRes;
    LPPLLOCALVARS lpplLocalVars;        // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // The strand needs to be saved to global memory

    // Save the base of this strand
    lpYYStrand                        =
    lpYYRes->unYYSTYPE.lpYYStrandBase = lpYYArg->unYYSTYPE.lpYYStrandBase;
    lpYYRes->lpYYFcn = (LPYYSTYPE) -1;  // For debugging

    // Get the # elements in the strand
    iLen = lpplLocalVars->lpYYStrandNext[VARSTRAND] - lpYYStrand;

    // Trundle through the strand stack converting
    //   to a common memory allocation type
    // Note that, although it doesn't make any difference here,
    //   we look at the elements in the reverse
    //   order they are on the stack because
    //   we parsed the tokenization from right
    //   to left.
    for (lpYYToken = &lpYYStrand[iLen - 1];
         bRet && lpYYToken NE &lpYYStrand[-1];
         lpYYToken--)
    {
        // Split cases based upon the token type
        switch (lpYYToken->tkToken.tkFlags.TknType)
        {
            case TKT_VARNAMED:
                // tkData is LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                // Check for VALUE ERROR
                if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Value)
                {
                    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                                              &lpYYToken->tkToken);
                    // Mark as in error
                    bRet = FALSE;

                    goto ERROR_EXIT;
                } // End IF

                if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                {
                    // Get the global handle
                    hGlbData = lpYYToken->tkToken.tkData.tkSym->stData.stGlbData;

                    // stData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir (hGlbData));

                    // Lock the memory to get a ptr to it
                    lpMemStr = MyGlobalLock (ClrPtrTypeDirGlb (hGlbData));

#define lpHeader    ((LPVARARRAY_HEADER) lpMemStr)

                    if (lpHeader->Rank EQ 0)
                        cStrandNxtType = TranslateArrayTypeToStrandType (lpHeader->ArrType);
                    else
                        cStrandNxtType = STRAND_NESTED;
#undef  lpHeader
                    // We no longer need this ptr
                    MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbData)); lpMemStr = NULL;

                    break;
                } // End IF

                // Handle the immediate case

                // tkData is LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                // stData is an immediate
                Assert (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm);

                switch (lpYYToken->tkToken.tkData.tkSym->stFlags.ImmType)
                {
                    case IMMTYPE_BOOL:
                        cStrandNxtType = STRAND_BOOL;

                        break;

                    case IMMTYPE_INT:
                        cStrandNxtType = STRAND_INT;

                        break;

                    case IMMTYPE_CHAR:
                        cStrandNxtType = STRAND_CHAR;

                        break;

                    case IMMTYPE_FLOAT:
                        cStrandNxtType = STRAND_FLOAT;

                        break;

                    defstop
                        return NULL;
                } // End SWITCH

                break;

            case TKT_VARIMMED:
                cStrandNxtType = TranslateImmTypeToStrandType (lpYYToken->tkToken.tkFlags.ImmType);

                break;

            case TKT_STRING:
                cStrandNxtType = STRAND_STRING;

                break;

            case TKT_VARARRAY:
                cStrandNxtType = STRAND_NESTED;

                break;

            case TKT_LISTPAR:
            case TKT_LISTBR:
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                          &lpYYArg->tkToken);
                // Mark as in error
                bRet = FALSE;

                goto ERROR_EXIT;

            case TKT_ASSIGN:
            case TKT_LISTSEP:
            case TKT_FCNIMMED:
            case TKT_COMMENT:
            case TKT_OP1IMMED:
            case TKT_OP2IMMED:
            case TKT_OPJOTDOT:
            case TKT_LPAREN:
            case TKT_RPAREN:
            case TKT_LBRACKET:
            case TKT_RBRACKET:
            case TKT_EOS:
            case TKT_EOL:
            case TKT_LINECONT:
            case TKT_STRAND:
            defstop
                return NULL;
        } // End SWITCH

        // Run through the conversion table
        cStrandCurType = tabConvert[cStrandCurType][cStrandNxtType];
    } // End FOR

    if (!bRet)
        goto ERROR_EXIT;

    aplType = TranslateStrandTypeToArrayType (cStrandCurType);

    //***********************************************************************
    //********** Single Element Case ****************************************
    //***********************************************************************

    // If there is a single element and the strand type is Boolean,
    //   Integer, Float, or Character (but not Charst), store it in
    //   a TKT_VARIMMED token, or if it's a string, pass on the existing
    //   HGLOBAL.
    if (iLen EQ 1)
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
                        Assert (GetPtrTypeDir (lpYYStrand->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                        // Pass through the entire token
                        YYCopy (lpYYRes, lpYYStrand);

                        // If it's not an immediate, ...
                        if (!lpYYRes->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // stData is a valid HGLOBAL variable array
                            Assert (IsGlbTypeVarDir (lpYYRes->tkToken.tkData.tkSym->stData.stGlbData));
                        } // End IF

                        break;

                    case TKT_VARIMMED:
                        // Pass through the entire token
                        YYCopy (lpYYRes, lpYYStrand);

                        break;

                    case TKT_STRING:
                        // Fill in the result token
                        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
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
                        Assert (GetPtrTypeDir (lpYYStrand->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                        // It's not immediate (handled above)
                        Assert (!lpYYStrand->tkToken.tkData.tkSym->stFlags.Imm);

                        // stData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (lpYYStrand->tkToken.tkData.tkSym->stData.stGlbData));

                        // Pass through the entire token
                        YYCopy (lpYYRes, lpYYStrand);

                        break;

                    case TKT_VARARRAY:
                        // tkData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (lpYYStrand->tkToken.tkData.tkGlbData));

                        // Fill in the result token
                        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
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
////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
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
    ByteRes = CalcArraySize (aplType, iLen, 1);

    // Allocate global memory for a length <iLen> vector of type <cState>
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbStr = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbStr)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYArg->tkToken);
        // Mark as in error
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbStr);
    lpYYRes->tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

    // Lock the global memory to get a ptr to it
    lpMemStr = MyGlobalLock (hGlbStr);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemStr)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType = aplType;
////lpHeader->Perm    = 0;
////lpHeader->SysVar  = 0;
    lpHeader->RefCnt  = 1;
    lpHeader->NELM    = iLen;
    lpHeader->Rank    = 1;

#undef  lpHeader

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

            for (lpYYToken = &lpYYStrand[iLen - 1];
                 lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            {
                // Split cases based upon the token type
                switch (lpYYToken->tkToken.tkFlags.TknType)
                {
                    case TKT_VARNAMED:
                        // tkData is an LPSYMENTRY
                        Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                        // If it's an immediate, ...
                        if (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // Copy the Boolean value to the result
                            *LPAPL.Bool |= (lpYYToken->tkToken.tkData.tkSym->stData.stBoolean) << iBitIndex++;
                        } else
                        {
                            // stData is an HGLOBAL
                            Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkSym->stData.lpVoid) EQ PTRTYPE_HGLOBAL);

                            DbgBrk ();          // ***FINISHME*** -- can we ever get here??
                                                //   Only if we allow a named var to point to
                                                //   a SYMENTRY which contains an HGLOBAL
                                                //   simple scalar.






                        } // End IF/ELSE

                        break;

                    case TKT_VARIMMED:
                        // Copy the Boolean value to the result
                        *LPAPL.Bool |= (lpYYToken->tkToken.tkData.tkBoolean) << iBitIndex++;

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
            for (lpYYToken = &lpYYStrand[iLen - 1];
                 lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            // Split cases based upon the token type
            switch (lpYYToken->tkToken.tkFlags.TknType)
            {
                case TKT_VARNAMED:
                    // tkData is an LPSYMENTRY
                    Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                    // If it's an immediate, ...
                    if (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                    {
                        // Copy the integer value to the result
                        *LPAPL.Int++ = lpYYToken->tkToken.tkData.tkSym->stData.stInteger;
                    } else
                    {
                        // stData is an HGLOBAL
                        Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkSym->stData.lpVoid) EQ PTRTYPE_HGLOBAL);

                        DbgBrk ();              // ***FINISHME*** -- can we ever get here??
                                                //   Only if we allow a named var to point to
                                                //   a SYMENTRY which contains an HGLOBAL
                                                //   simple scalar.






                    } // End IF/ELSE

                    break;

                case TKT_VARIMMED:
                    // Copy the integer value to the result
                    *LPAPL.Int++ = lpYYToken->tkToken.tkData.tkInteger;

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case STRAND_CHAR:
        case STRAND_CHARST:         // lpYYToken->tkToken.tkData.tkChar (TKT_VARIMMED)
            for (lpYYToken = &lpYYStrand[iLen - 1];
                 lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            // Split cases based upon the token type
            switch (lpYYToken->tkToken.tkFlags.TknType)
            {
                case TKT_VARNAMED:
                    // tkData is an LPSYMENTRY
                    Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                    // If it's an immediate, ...
                    if (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                    {
                        // Copy the char value to the result
                        *LPAPL.Char++ = lpYYToken->tkToken.tkData.tkSym->stData.stChar;
                    } else
                    {
                        // stData is an HGLOBAL
                        Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkSym->stData.lpVoid) EQ PTRTYPE_HGLOBAL);

                        DbgBrk ();              // ***FINISHME*** -- can we ever get here??
                                                //   Only if we allow a named var to point to
                                                //   a SYMENTRY which contains an HGLOBAL
                                                //   simple scalar.






                    } // End IF/ELSE

                    break;

                case TKT_VARIMMED:
                    // Copy the char value to the result
                    *LPAPL.Char++ = lpYYToken->tkToken.tkData.tkChar;

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case STRAND_FLOAT:          // lpYYToken->tkToken.tkData.tkFloat (TKT_VARIMMED)
            for (lpYYToken = &lpYYStrand[iLen - 1];
                 bRet && lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            // Split cases based upon the token type
            switch (lpYYToken->tkToken.tkFlags.TknType)
            {
                case TKT_VARNAMED:
                    // tkData is an LPSYMENTRY
                    Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

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
                            // ***FIXME*** -- Possible loss of precision

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

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        case STRAND_HETERO:     // lpYYToken->tkToken.tkData.tkSym (LPSYMENTRY)
        case STRAND_NESTED:     // lpYYToken->tkToken.tkData.aplNested (LPSYMENTRY or HGLOBAL)
            for (lpYYToken = &lpYYStrand[iLen - 1];
                 bRet && lpYYToken NE &lpYYStrand[-1];
                 lpYYToken--)
            {
                switch (lpYYToken->tkToken.tkFlags.TknType)
                {
                    case TKT_VARNAMED:    // 1 a
                    {
                        LPSYMENTRY lpSym;

                        // tkData is an LPSYMENTRY
                        Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                        // If the STE is immediate, make a copy of it
                        if (lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // Make a copy of the symbol table entry as we can't use the
                            //   one in the STNAME.
                            lpSym = CopyImmSymEntry_EM (lpYYToken->tkToken.tkData.tkSym,
                                                        -1,
                                                        &lpYYToken->tkToken);
                            if (lpSym)
                                // Save the symbol table entry and skip past it
                                *LPAPL.Sym++ = (LPSYMENTRY) MakeSymType (lpSym);
                            else
                                bRet = FALSE;
                        } else
                        {
                            // stData is a valid HGLOBAL variable array
                            Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.tkSym->stData.stGlbData));

                            *LPAPL.Nested++ = CopySymGlbDir (lpYYToken->tkToken.tkData.tkSym->stData.stGlbData);
                        } // End IF/ELSE

                        break;
                    } // End TKT_VARNAMED

                    case TKT_VARIMMED:  // 1.5 'ab'
                    {
                        LPSYMENTRY lpSymEntry;

                        // Copy the immediate token as an LPSYMENTRY
                        lpSymEntry = CopyImmToken_EM (&lpYYToken->tkToken);
                        if (lpSymEntry)
                            *LPAPL.Nested++ = lpSymEntry;
                        else
                            bRet = FALSE;
                        break;
                    } // End TKT_VARIMMED

                    case TKT_VARARRAY:  // 1('ab')
                    case TKT_STRING:    // 1 'ab'
                        // tkData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.tkGlbData));

                        // Copy the nested entry to the result
                        *LPAPL.Nested++ = lpYYToken->tkToken.tkData.tkGlbData;

                        // Mark as reused
                        lpYYToken->tkToken.tkData.tkGlbData = PTR_REUSED;

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case STRAND_STRING:
        defstop
            break;
    } // End FOR/SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbStr); lpMemStr = NULL;
NORMAL_EXIT:
    // Free the tokens on this portion of the strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[VARSTRAND], lpplLocalVars->lpYYStrandBase[VARSTRAND]);

    // Strip the tokens on this portion of the strand stack
    StripStrand (lpYYRes, VARSTRAND);

    DBGLEAVE;

    return lpYYRes;

ERROR_EXIT:
    // Free the entire strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[VARSTRAND], lpplLocalVars->lpYYStrandStart[VARSTRAND]);

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

LPYYSTYPE MakeFcnStrand_EM_YY
    (LPYYSTYPE     lpYYArg,         // Ptr to incoming token
     FCN_TYPES     cFcnType)        // Type of the strand

{
    int           iIniLen,
                  iActLen,
                  FcnCount = 0;
    APLUINT       ByteRes;                  // # bytes needed for the result
    LPYYSTYPE     lpYYStrand;
    HGLOBAL       hGlbStr;
    LPVOID        lpMemStr;
    LPYYSTYPE     lpYYMemStart,
                  lpYYMemData,
                  lpYYBase = (LPYYSTYPE) -1,
                  lpYYRes;
    BOOL          bRet = TRUE;
    LPPLLOCALVARS lpplLocalVars;   // Ptr to local plLocalVars

    DBGENTER;

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Save the base of this strand
    lpYYStrand                        =
    lpYYRes->unYYSTYPE.lpYYStrandBase = lpYYArg->unYYSTYPE.lpYYStrandBase;

    // Get the (maximum) # elements in the strand
    iIniLen = lpplLocalVars->lpYYStrandNext[FCNSTRAND] - lpYYStrand;

    //***********************************************************************
    //********** Single Element Case ****************************************
    //***********************************************************************

    // If there is a single element, pass through the entire token
    if (iIniLen EQ 1)
    {
        // Free the allocated result as CopyYYSTYPE_EM_YY
        //   will allocate a result
        YYFree (lpYYRes); lpYYRes = NULL;

        // Copy the entire token
        lpYYRes = CopyYYSTYPE_EM_YY (lpYYArg->lpYYFcn, FALSE);
        lpYYRes->FcnCount = 1;

        lpYYBase = lpYYArg->lpYYFcn;

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
    ByteRes = sizeof (FCNARRAY_HEADER)      // For the header
            + sizeof (YYSTYPE) * iIniLen;   // For the data

    // Allocate global memory for a length <iLen> vector of type <YYSTYPE>.
    // N.B.: Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbStr = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbStr)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYArg->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_FCNARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;    // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;    // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbStr);
    lpYYRes->tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;

    // Lock the global memory to get a ptr to it
    lpMemStr = MyGlobalLock (hGlbStr);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemStr)

    // Fill in the header
    lpHeader->Sign.ature = FCNARRAY_HEADER_SIGNATURE;
    lpHeader->FcnType    = cFcnType;
    lpHeader->RefCnt     = 1;
////lpHeader->NELM       =              // To be filled in below

#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpYYMemStart = lpYYMemData = FcnArrayBaseToData (lpMemStr);

    // Copy the YYSTYPEs to the global memory object
    lpYYMemData = CopyYYFcn (lpYYMemData, lpYYArg->lpYYFcn, &lpYYBase, &FcnCount);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemStr)

    lpHeader->NELM = iActLen = lpYYMemData - lpYYMemStart;

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbStr); lpMemStr = lpYYMemData = lpYYMemStart = NULL;

    // Handle case where we overallocated to the extent that there
    //   is only one function which can then be immediate.
    if (iActLen EQ 1)
    {
        // Copy the entire token
        CopyYYFcn (lpYYRes, lpYYArg->lpYYFcn, &lpYYBase, &FcnCount);
        Assert (FcnCount EQ 1);

        // Note that the result token in lpYYRes is now TKT_FCNIMMED

        // We no longer need this storage
        FreeResultGlobalFcn (hGlbStr); hGlbStr = NULL;
    } else
    if (iIniLen NE iActLen)
    {
        // Resize the block downwards
        hGlbStr =
        MyGlobalReAlloc (hGlbStr,
                         (UINT) ByteRes - (iIniLen - iActLen) * sizeof (YYSTYPE),
                         GHND);
    } // End IF/ELSE/IF

    // Save the token & function counts
    lpYYRes->TknCount = iActLen;
    lpYYRes->FcnCount = FcnCount;
NORMAL_EXIT:
    lpYYRes->unYYSTYPE.lpYYStrandBase  = lpplLocalVars->lpYYStrandBase[FCNSTRAND] = lpYYBase;

#ifdef DEBUG
    DisplayStrand (FCNSTRAND);
#endif

    // Free the tokens on this portion of the strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[FCNSTRAND], lpplLocalVars->lpYYStrandBase[FCNSTRAND]);

    // Strip the tokens on this portion of the strand stack
    StripStrand (lpYYRes, FCNSTRAND);

    DBGLEAVE;

    return lpYYRes;

ERROR_EXIT:
    // Free the entire function strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[FCNSTRAND], lpplLocalVars->lpYYStrandStart[FCNSTRAND]);

    DBGLEAVE;

    YYFree (lpYYRes); lpYYRes = NULL; return NULL;
} // End MakeFcnStrand_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $CopyYYFcn
//
//  Copy one or more YYSTYPE functions to a memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyYYFcn"
#else
#define APPEND_NAME
#endif

LPYYSTYPE CopyYYFcn
    (LPYYSTYPE  lpYYMem,            // Ptr to result memory object
     LPYYSTYPE  lpYYArg,            // Ptr to function arg
     LPYYSTYPE *lpYYBase,           // Ptr to ptr to YY base address
     LPINT      lpFcnCount)         // Ptr to resulting function count

{
    int       i,
              iLen,
              FcnCount,
              TotalFcnCount = 0;
    YYSTYPE   YYFcn = {0};
    HGLOBAL   hGlbData;
    LPTOKEN   lpToken;
    LPYYSTYPE lpYYMem0,
              lpYYCopy;

    // Get the token count in this function strand
    iLen = lpYYArg->TknCount;

    // We need to modify the function count in the first token,
    //   so save its address in the array.
    lpYYMem0 = lpYYMem;

    // Loop through the tokens associated with this symbol
    for (i = 0; i < iLen; i++)
    {
        lpToken = &lpYYArg[i].tkToken;

        // Calculate the earlier function base
        *lpYYBase = min (*lpYYBase, lpYYArg[i].lpYYFcn);

        // Special case for named functions/operators
        if (lpToken->tkFlags.TknType EQ TKT_FCNNAMED
         || lpToken->tkFlags.TknType EQ TKT_OP1NAMED
         || lpToken->tkFlags.TknType EQ TKT_OP2NAMED)
        {
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's an immediate function/operator, copy it directly
            if (lpToken->tkData.tkSym->stFlags.Imm)
            {
                YYFcn = lpYYArg[i];
                YYFcn.tkToken.tkFlags.TknType   = TranslateImmTypeToTknType (lpToken->tkData.tkSym->stFlags.ImmType);
                YYFcn.tkToken.tkFlags.ImmType   = lpToken->tkData.tkSym->stFlags.ImmType;
////////////////YYFcn.tkToken.tkFlags.NoDisplay = 0;        // Already zero from = {0}
                YYFcn.tkToken.tkData.tkLongest  = 0;        // Keep the extraneous data clear
                YYFcn.tkToken.tkData.tkChar     = lpToken->tkData.tkSym->stData.stChar;
////////////////YYFcn.tkToken.tkCharIndex       =
////////////////YYFcn.TknCount                  = lpYYArg[i].TknCount; // (Factored out below)
////////////////YYFcn.FcnCount                  = FcnCount;            // (Factored out below)
////////////////YYFcn.Indirect                  =
////////////////YYFcn.lpYYFcn                   =
////////////////YYFcn.unYYSTYPE.lpYYStrandBase  =
            } else
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                //stData is a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir (hGlbData));

                // Increment the reference count in global memory
                DbgIncrRefCntDir (hGlbData);

                YYFcn = lpYYArg[i];
                YYFcn.tkToken.tkFlags.TknType   = TKT_FCNARRAY;
////////////////YYFcn.tkToken.tkFlags.ImmType   = 0;        // Already zero from = {0}
////////////////YYFcn.tkToken.tkFlags.NoDisplay = 0;        // Already zero from = {0}
                YYFcn.tkToken.tkData.tkGlbData  = hGlbData;
////////////////YYFcn.tkToken.tkCharIndex       =
////////////////YYFcn.TknCount                  = lpYYArg[i].TknCount; // (Factored out below)
////////////////YYFcn.FcnCount                  = FcnCount;            // (Factored out below)
////////////////YYFcn.Indirect                  =
////////////////YYFcn.lpYYFcn                   =
////////////////YYFcn.unYYSTYPE.lpYYStrandBase  =
            } // End IF/ELSE

            FcnCount = 1;
            YYFcn.TknCount = lpYYArg[i].TknCount;
            YYFcn.FcnCount = FcnCount;
            YYFcn.Inuse = 0;
#ifdef DEBUG
            YYFcn.Index = (UINT) -1;
#endif
            *lpYYMem++ = YYFcn;

            // Save the function count
            lpYYMem[-1].FcnCount = FcnCount;
        } else
        if (lpYYArg[i].Indirect)
        {
            FcnCount = 0;
            lpYYMem = CopyYYFcn (lpYYMem, lpYYArg[i].lpYYFcn, lpYYBase, &FcnCount);
        } else
        {
            FcnCount = 1;
            lpYYCopy = CopyYYSTYPE_EM_YY (&lpYYArg[i], FALSE);
            if (lpYYMem->Inuse)
                YYCopy (lpYYMem++, lpYYCopy);
            else
                YYCopyFreeDst (lpYYMem++, lpYYCopy);
            YYFree (lpYYCopy); lpYYCopy = NULL;

            // Save the function count
            lpYYMem[-1].FcnCount = FcnCount;
        } // End IF/ELSE/...

        TotalFcnCount += FcnCount;
    } // End FOR

    // Save the total function count in the first token
    lpYYMem0->FcnCount = TotalFcnCount;

    // Return as the overall total
    *lpFcnCount = TotalFcnCount;

#ifdef DEBUG
    lpYYArg[0].FcnCount = TotalFcnCount;
#endif
    return lpYYMem;
} // End CopyYYFcn
#undef  APPEND_NAME


//***************************************************************************
//  $ErrorMessageIndirectToken
//
//  Signal an error message, indirectly
//    and set the error token
//***************************************************************************

void ErrorMessageIndirectToken
    (WCHAR   *lpwszMsg,
     LPTOKEN  lpError)

{
    // Save in global for later reference
    lpwszErrorMessage = lpwszMsg;

    // Set the error token
    ErrorMessageSetToken (lpError);
} // End ErrorMessageIndirectToken


//***************************************************************************
//  $ErrorMessageSetToken
//
//  Set the error token for an error message
//***************************************************************************

void ErrorMessageSetToken
    (LPTOKEN lpError)

{
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Set the error token
    lpplLocalVars->tkErrorCharIndex = lpError->tkCharIndex;
} // End ErrorMessageSetToken


//***************************************************************************
//  $CopyString_EM_YY
//
//  Copy a string value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyString_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE CopyString_EM_YY
    (LPYYSTYPE lpYYStr)

{
    LPYYSTYPE lpYYRes;

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (lpYYStr->tkToken.tkData.tkGlbData));

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
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

LPYYSTYPE MakeAxis_YY
    (LPYYSTYPE lpYYAxis)    // Ptr to axis value

{
    LPYYSTYPE lpYYRes;      // Ptr to result

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Split cases based upon the token type
    switch (lpYYAxis->tkToken.tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYAxis->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate, ...
            if (!lpYYAxis->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lpYYAxis->tkToken.tkData.tkSym->stData.stGlbData));

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_AXISARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir (lpYYAxis->tkToken.tkData.tkSym->stData.stGlbData);
                lpYYRes->tkToken.tkCharIndex       = lpYYAxis->tkToken.tkCharIndex;

                break;
            } // End IF

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_AXISIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = lpYYAxis->tkToken.tkData.tkSym->stFlags.ImmType;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkLongest  = lpYYAxis->tkToken.tkData.tkSym->stData.stLongest;
            lpYYRes->tkToken.tkCharIndex       = lpYYAxis->tkToken.tkCharIndex;

            break;

        case TKT_VARIMMED:
            // Copy the token and rename it
            YYCopy (lpYYRes, lpYYAxis);     // No need to CopyYYSTYPE_EM_YY immediates
            lpYYRes->tkToken.tkFlags.TknType = TKT_AXISIMMED;

            break;

        case TKT_VARARRAY:
            // Free the result as CopyYYSTYPE_EM_YY
            //   will allocate a result
            YYFree (lpYYRes); lpYYRes = NULL;

            // Copy the token and rename it
            lpYYRes = CopyYYSTYPE_EM_YY (lpYYAxis, FALSE);
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

LPYYSTYPE MakePrimFcn_YY
    (LPYYSTYPE lpYYFcn)

{
    LPYYSTYPE lpYYRes;

    DBGENTER;

    lpYYRes = CopyYYSTYPE_EM_YY (lpYYFcn, FALSE);
    lpYYRes->tkToken.tkFlags.TknType = TKT_FCNIMMED;
    lpYYRes->tkToken.tkFlags.ImmType = GetImmTypeFcn (lpYYFcn->tkToken.tkData.tkChar);
    lpYYRes->lpYYFcn = NULL;
    lpYYRes->FcnCount = 1;

    DBGLEAVE;

    return lpYYRes;
} // End MakePrimFcn_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeNameFcn_YY
//
//  Make a token for a named function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeNameFcn_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeNameFcn_YY
    (LPYYSTYPE lpYYFcn)

{
    LPYYSTYPE lpYYRes;

    DBGENTER;

    // Because when the tokens are first created we don't
    //   know the type of a name, it is arbitrarily typed
    //   as TKT_VARNAMED.  Now that we know it's a function,
    //   change its type to TKT_FCNNAMED.
    lpYYFcn->tkToken.tkFlags.TknType = TKT_FCNNAMED;

    // If it's a System Function, mark is as direct
    if (lpYYFcn->tkToken.tkData.tkSym->stFlags.SysFn12)
        lpYYFcn->tkToken.tkFlags.FcnDir = 1;

    lpYYRes = CopyYYSTYPE_EM_YY (lpYYFcn, FALSE);
    lpYYRes->lpYYFcn = NULL;

    DBGLEAVE;

    return lpYYRes;
} // End MakeNameFcn_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeOp1_YY
//
//  Make a monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeOp1_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeOp1_YY
    (LPYYSTYPE lpYYOp1)

{
    LPYYSTYPE lpYYRes;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    YYCopy (lpYYRes, lpYYOp1);      // No need to CopyYYSTYPE_EM_YY immediates
    lpYYRes->tkToken.tkFlags.TknType = TKT_OP1IMMED;
    lpYYRes->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP1;
    lpYYRes->lpYYFcn                 = NULL;
    lpYYRes->TknCount                =
    lpYYRes->FcnCount                =
    lpYYRes->Rsvd                    = 0;
#ifdef DEBUG
    lpYYRes->Index                   = (UINT) -1;
    lpYYRes->Flag                    = 0;
#endif
    return lpYYRes;
} // End MakeOp1_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeNameOp1_YY
//
//  Make a named monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeNameOp1_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeNameOp1_YY
    (LPYYSTYPE lpYYOp1)

{
    LPYYSTYPE lpYYRes;

    // Because when the tokens are first created we don't
    //   know the type of a name, it is arbitrarily typed
    //   as TKT_VARNAMED.  Now that we know it's a monadic operator,
    //   change its type to TKT_OP1NAMED.

    lpYYOp1->tkToken.tkFlags.TknType = TKT_OP1NAMED;

    lpYYRes = CopyYYSTYPE_EM_YY (lpYYOp1, FALSE);
    lpYYRes->tkToken.tkFlags.TknType = TKT_OP1NAMED;
    lpYYRes->lpYYFcn                 = NULL;
    lpYYRes->TknCount                =
    lpYYRes->FcnCount                =
    lpYYRes->Rsvd                    = 0;
#ifdef DEBUG
    lpYYRes->Index                   = (UINT) -1;
    lpYYRes->Flag                    = 0;
#endif
    return lpYYRes;
} // End MakeNameOp1_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeOp2_YY
//
//  Make a dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeOp2_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeOp2_YY
    (LPYYSTYPE lpYYOp2)

{
    LPYYSTYPE lpYYRes;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    YYCopy (lpYYRes, lpYYOp2);      // No need to CopyYYSTYPE_EM_YY immediates
    lpYYRes->tkToken.tkFlags.TknType = TKT_OP2IMMED;
    lpYYRes->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP2;
    lpYYRes->lpYYFcn                 = NULL;
    lpYYRes->TknCount                =
    lpYYRes->FcnCount                =
    lpYYRes->Rsvd                    = 0;
#ifdef DEBUG
    lpYYRes->Index                   = (UINT) -1;
    lpYYRes->Flag                    = 0;
#endif
    return lpYYRes;
} // End MakeOp2_YY
#undef  APPEND_NAME


//***************************************************************************
//  $MakeNameOp2_YY
//
//  Make a named dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeNameOp2_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeNameOp2_YY
    (LPYYSTYPE lpYYOp2)

{
    LPYYSTYPE lpYYRes;

    // Because when the tokens are first created we don't
    //   know the type of a name, it is arbitrarily typed
    //   as TKT_VARNAMED.  Now that we know it's a dyadic operator,
    //   change its type to TKT_OP2NAMED.

    lpYYOp2->tkToken.tkFlags.TknType = TKT_OP2NAMED;

    lpYYRes = CopyYYSTYPE_EM_YY (lpYYOp2, FALSE);
    lpYYRes->tkToken.tkFlags.TknType = TKT_OP2NAMED;
    lpYYRes->lpYYFcn                 = NULL;
    lpYYRes->TknCount                =
    lpYYRes->FcnCount                =
    lpYYRes->Rsvd                    = 0;
#ifdef DEBUG
    lpYYRes->Index                   = (UINT) -1;
    lpYYRes->Flag                    = 0;
#endif
    return lpYYRes;
} // End MakeNameOp2_YY
#undef  APPEND_NAME


//***************************************************************************
//  $InitNameStrand
//
//  Initialize a name strand
//***************************************************************************

void InitNameStrand
    (LPYYSTYPE lpYYArg)             // Ptr to the incoming argument

{
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Set the base of this strand to the next available location
    lpYYArg->unYYSTYPE.lpYYStrandBase        =
    lpplLocalVars->lpYYStrandBase[VARSTRAND] = lpplLocalVars->lpYYStrandNext[VARSTRAND];
} // End InitNameStrand


//***************************************************************************
//  $PushNameStrand_YY
//
//  Push a name strand
//***************************************************************************

LPYYSTYPE PushNameStrand_YY
    (LPYYSTYPE lpYYArg)             // Ptr to the incoming argument

{
    LPYYSTYPE lpYYRes;
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_STRAND;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.lpVoid     = (LPVOID) -1;
    lpYYRes->tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;

    lpYYRes->unYYSTYPE.lpYYStrandBase  =
    lpYYArg->unYYSTYPE.lpYYStrandBase  = lpplLocalVars->lpYYStrandBase[VARSTRAND];

    // Save this token on the strand stack
    //   and skip over it
    YYCopyFreeDst (lpplLocalVars->lpYYStrandNext[VARSTRAND]++, lpYYArg);

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (VARSTRAND);
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

LPYYSTYPE MakeNameStrand_EM_YY
    (LPYYSTYPE lpYYArg)             // Ptr to incoming token

{
    int           iLen;
    APLUINT       ByteRes;          // # bytes needed for the result
    LPYYSTYPE     lpYYStrand;
    HGLOBAL       hGlbStr;
    LPVOID        lpMemStr;
    BOOL          bRet = TRUE;
    LPYYSTYPE     lpYYRes;          // Ptr to result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Save the base of this strand
    lpYYStrand                        =
    lpYYRes->unYYSTYPE.lpYYStrandBase = lpYYArg->unYYSTYPE.lpYYStrandBase;

    // Get the # elements in the strand
    iLen = lpplLocalVars->lpYYStrandNext[VARSTRAND] - lpYYStrand;

    // Save these tokens in global memory

    // Calculate storage needed for the tokens
    ByteRes = sizeof (VARNAMED_HEADER)          // For the header
            + sizeof (lpYYStrand[0]) * iLen;    // For the data

    // Allocate global memory for a length <iLen> vector of type <cState>
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbStr = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbStr)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYArg->tkToken);
        // Mark as in error
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_STRNAMED;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbStr);
    lpYYRes->tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

    // Lock the global memory to get a ptr to it
    lpMemStr = MyGlobalLock (hGlbStr);

#define lpHeader    ((LPVARNAMED_HEADER) lpMemStr)

    // Fill in the header
    lpHeader->Sign.ature = VARNAMED_HEADER_SIGNATURE;
    lpHeader->NELM       = iLen;

#undef  lpHeader

    // Skip over the header to the data
    lpMemStr = VarNamedBaseToData (lpMemStr);

    // Copy the tokens to global memory
    CopyMemory (lpMemStr, lpYYStrand, iLen * sizeof (lpYYStrand[0]));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbStr); lpMemStr = NULL;

    // Free the tokens on this portion of the strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[VARSTRAND], lpplLocalVars->lpYYStrandBase[VARSTRAND]);

    // Strip the tokens on this portion of the strand stack
    StripStrand (lpYYRes, VARSTRAND);

    DBGLEAVE;

    return lpYYRes;

ERROR_EXIT:
    // Free the entire strand stack
    FreeStrand (lpplLocalVars->lpYYStrandNext[VARSTRAND], lpplLocalVars->lpYYStrandStart[VARSTRAND]);

    DBGLEAVE;

    YYFree (lpYYRes); lpYYRes = NULL; return NULL;
} // End MakeNameStrand_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $InitList0_YY
//
//  Initialize a list starting with an empty token
//***************************************************************************

LPYYSTYPE InitList0_YY
    (void)

{
    LPYYSTYPE lpYYRes,
              lpYYLst;
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_LISTINT;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.lpVoid     = (LPVOID) -1;
    lpYYRes->tkToken.tkCharIndex       = (UINT) -1 ;

    // Set the base of this strand to the next available location
    lpYYRes->unYYSTYPE.lpYYStrandBase        =
    lpplLocalVars->lpYYStrandBase[VARSTRAND] = lpplLocalVars->lpYYStrandNext[VARSTRAND];

    lpYYLst = PushList_YY (lpYYRes, NULL);
    FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;

    return lpYYLst;
} // End InitList0_YY


//***************************************************************************
//  $InitList1_YY
//
//  Initialize a list starting with a single token
//***************************************************************************

LPYYSTYPE InitList1_YY
    (LPYYSTYPE lpYYArg)             // Ptr to incoming token

{
    LPYYSTYPE lpYYRes,
              lpYYLst;
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_LISTINT;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.lpVoid     = (LPVOID) -1;
    lpYYRes->tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;

    // Set the base of this strand to the next available location
    lpYYRes->unYYSTYPE.lpYYStrandBase        =
    lpplLocalVars->lpYYStrandBase[VARSTRAND] = lpplLocalVars->lpYYStrandNext[VARSTRAND];

    lpYYLst = PushList_YY (lpYYRes, lpYYArg);
    FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;

    return lpYYLst;
} // End InitList1_YY


//***************************************************************************
//  $PushList_YY
//
//  Push a token onto the list stack
//***************************************************************************

LPYYSTYPE PushList_YY
    (LPYYSTYPE lpYYStrand,          // Ptr to base of strand
     LPYYSTYPE lpYYArg)             // Ptr to incoming token

{
    LPYYSTYPE     lpYYRes;
    YYSTYPE       YYTmp;
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    YYCopy (lpYYRes, lpYYStrand);

    // If the token is NULL, push an empty token
    if (lpYYArg EQ NULL)
    {
        YYTmp.tkToken.tkFlags.TknType   = TKT_LISTSEP;
        YYTmp.tkToken.tkData.lpVoid     = (LPVOID) -1;
        YYTmp.tkToken.tkCharIndex       = (UINT) -1;
        YYTmp.unYYSTYPE.lpYYStrandBase  = lpplLocalVars->lpYYStrandBase[VARSTRAND];
        lpYYArg = &YYTmp;
    } // End IF

    // Save this token on the strand stack
    //   and skip over it
    YYCopyFreeDst (lpplLocalVars->lpYYStrandNext[VARSTRAND]++, lpYYArg);

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand (VARSTRAND);
#endif
    return lpYYRes;
} // End PushList_YY


//***************************************************************************
//  $MakeList_EM_YY
//
//  Make the listy into a global memory array.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeList_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeList_EM_YY
    (LPYYSTYPE lpYYArg,             // Ptr to incoming token
     BOOL      bBrackets)           // TRUE iff surrounding brackets (otherwise parens)

{
    LPYYSTYPE  lpYYStrand,
               lpYYToken;
    int        iLen;
    APLUINT    ByteRes;             // # bytes needed for the result
    HGLOBAL    hGlbLst;
    LPVOID     lpMemLst;
    LPSYMENTRY lpSymEntry;
    BOOL       bRet = TRUE;
    LPYYSTYPE  lpYYRes;
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsLocalVars);

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // The list needs to be saved to global memory

    // Save the base of this strand
    lpYYStrand                        =
    lpYYRes->unYYSTYPE.lpYYStrandBase = lpYYArg->unYYSTYPE.lpYYStrandBase;

    // Get the # elements in the strand
    iLen = lpplLocalVars->lpYYStrandNext[VARSTRAND] - lpYYStrand;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_NESTED, iLen, 1);

    // Allocate global memory for a length <iLen> vector of type <cState>
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbLst = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbLst)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYArg->tkToken);
        DBGLEAVE;

        // Mark as in error
        YYFree (lpYYRes); lpYYRes = NULL; return NULL;
    } // End IF

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = bBrackets ? TKT_LISTBR : TKT_LISTPAR;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbLst);
    lpYYRes->tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

    // Lock the global memory
    lpMemLst = MyGlobalLock (hGlbLst);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemLst)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_LIST;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = iLen;
    lpHeader->Rank       = 1;

#undef  lpHeader

    *VarArrayBaseToDim (lpMemLst) = iLen;

    // Skip over the header and one dimension (it's a vector)
    lpMemLst = VarArrayBaseToData (lpMemLst, 1);

    // Copy the elements to the global memory
    // Note we copy the elements in the reverse
    //   order they are on the stack because
    //   we parsed the tokenization from right
    //   to left.
    for (lpYYToken = &lpYYStrand[iLen - 1];
         bRet && lpYYToken NE &lpYYStrand[-1];
         lpYYToken--)
    {
        // Split cases based upon the token type
        switch (lpYYToken->tkToken.tkFlags.TknType)
        {
            case TKT_VARIMMED:
                // Copy the immediate token as an LPSYMENTRY
                lpSymEntry = CopyImmToken_EM (&lpYYToken->tkToken);
                if (lpSymEntry)
                    *((LPAPLLIST) lpMemLst)++ = lpSymEntry;
                else
                    bRet = FALSE;
                break;

            case TKT_VARARRAY:  // 1('ab')
            case TKT_STRING:    // 1 'ab'
                // Copy the nested entry to the result
                *((LPAPLLIST) lpMemLst)++ = MakeGlbTypeGlb (lpYYToken->tkToken.tkData.tkGlbData);

                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Unlock the handle
    MyGlobalUnlock (hGlbLst); lpMemLst = NULL;

    // Strip from the strand stack
    StripStrand (lpYYRes, VARSTRAND);

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
     BOOL    bChanging) // TRUE iff we're going to change the HGLOBAL

{
    static TOKEN tkRes;     // ***FIXME*** -- dynamic rather than static

    DBGENTER;

    // We haven't defined an instance of TRUE as yet
    Assert (bChanging EQ FALSE);

    tkRes = *lpToken;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:      // tkData is LPSYMENTRY
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lpToken->tkData.tkSym->stData.stGlbData));

                // Increment the reference count in global memory
                DbgIncrRefCntDir (lpToken->tkData.tkSym->stData.stGlbData);

                break;          // We're done
            } // End IF

            break;              // Ignore immediates

        case TKT_FCNNAMED:      // tkData is LPSYMENTRY
        case TKT_OP1NAMED:      // tkData is LPSYMENTRY
        case TKT_OP2NAMED:      // tkData is LPSYMENTRY
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                STFLAGS stFlags;

                // Skip assertion if it's some kind of function
                stFlags = lpToken->tkData.tkSym->stFlags;
                if (stFlags.SysFn0
                 || stFlags.SysFn12
                 || stFlags.UsrFn0
                 || stFlags.UsrFn12
                 || stFlags.UsrOp1
                 || stFlags.UsrOp2)
                    break;

                // stData is a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir (lpToken->tkData.tkSym->stData.stGlbData));

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
        case TKT_OPJOTDOT:      // ...
            break;              // Ignore immediates

        case TKT_LISTPAR:       // tkData is HGLOBAL
        case TKT_LISTBR:        // tkData is HGLOBAL
        defstop
            break;
    } // End SWITCH

    DBGLEAVE;

    return &tkRes;
} // End CopyToken_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CopyYYSTYPE_EM_YY
//
//  Make a copy of a YYSTYPE, incrementing ref count if not changing
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyYYSTYPE_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE CopyYYSTYPE_EM_YY
    (LPYYSTYPE lpYYArg,
     BOOL      bChanging)   // TRUE iff we're going to change the HGLOBAL

{
    LPYYSTYPE  lpYYRes;

    DBGENTER;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    Assert (bChanging EQ FALSE);

    // Copy the YYSTYPE
    YYCopy (lpYYRes, lpYYArg);

    // Make a copy of the token within
    lpYYRes->tkToken = *CopyToken_EM (&lpYYArg->tkToken, bChanging);

    DBGLEAVE;

    return lpYYRes;
} // End CopyYYSTYPE_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: strand.c
//***************************************************************************
