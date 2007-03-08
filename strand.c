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
//  InitVarStrand
//
//  Initialize the strand stack.
//***************************************************************************

void InitVarStrand
    (LPYYSTYPE lpYYArg)         // Ptr to the incoming argument

{
    // Set the base of this strand to the next available location
    lpYYArg->unYYSTYPE.lpYYStrandBase =
    gplLocalVars.lpYYStrandBase       = gplLocalVars.lpYYStrandNext;
} // End InitVarStrand


//***************************************************************************
//  PushVarStrand
//
//  Push a variable token onto the strand stack.
//***************************************************************************

LPYYSTYPE PushVarStrand
    (LPYYSTYPE lpYYArg)         // Ptr to the incoming argument

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType     = TKT_STRAND;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType     = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay   = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color       =
    YYRes[YYLclIndex].tkToken.tkData.lpVoid       = (LPVOID) -1;
    YYRes[YYLclIndex].tkToken.tkCharIndex         = lpYYArg->tkToken.tkCharIndex;

    YYRes[YYLclIndex].unYYSTYPE.lpYYStrandBase    =
    lpYYArg->unYYSTYPE.lpYYStrandBase = gplLocalVars.lpYYStrandBase;

    // Save this token on the strand stack
    //   and skip over it
    *gplLocalVars.lpYYStrandNext++ = *lpYYArg;

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand ();
#endif

    return &YYRes[YYLclIndex];
} // End PushVarStrand


//***************************************************************************
//  PushFcnStrand
//
//  Push a function token onto the strand stack.
//***************************************************************************

LPYYSTYPE PushFcnStrand
    (LPYYSTYPE lpYYArg,         // Ptr to the incoming argument
     int       TknCount,        // Token count
     BOOL      bIndirect)       // TRUE iff lpYYArg is indirect

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    lpYYArg->TknCount = TknCount;
    lpYYArg->Indirect = bIndirect;
    lpYYArg->unYYSTYPE.lpYYStrandBase = gplLocalVars.lpYYStrandBase;
    if (!lpYYArg->lpYYFcn)
        lpYYArg->lpYYFcn = gplLocalVars.lpYYStrandNext;

    // Fill in the result token
    YYRes[YYLclIndex] = *lpYYArg;

    // Return our own position so the next user
    //   of this token can refer to it.
    YYRes[YYLclIndex].lpYYFcn = gplLocalVars.lpYYStrandNext;

    // Save this token on the strand stack
    //   and skip over it
    *gplLocalVars.lpYYStrandNext++ = *lpYYArg;

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand ();
#endif

    return &YYRes[YYLclIndex];
} // End PushFcnStrand


//***************************************************************************
//  StripStrand
//
//  Strip a strand from the strand stack
//***************************************************************************

void StripStrand
    (LPYYSTYPE lpYYStrand)

{
    // If we're not back at the beginning, set the new base
    //   to the base of the token previous to the current base
    if (gplLocalVars.lpYYStrandBase NE gplLocalVars.lpYYStrandStart)
        gplLocalVars.lpYYStrandBase = gplLocalVars.lpYYStrandBase[-1].unYYSTYPE.lpYYStrandBase;

    // Set next available slot to this YYtoken's base
    gplLocalVars.lpYYStrandNext = lpYYStrand->unYYSTYPE.lpYYStrandBase;

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand ();
#endif
} // End StripStrand


//***************************************************************************
//  FreeStrand
//
//  Free the tokens on the strand stack
//***************************************************************************

void FreeStrand
    (LPYYSTYPE lpYYStrandNext,
     LPYYSTYPE lpYYStrand)

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
                if (!lpYYToken->tkToken.tkData.lpSym->stFlags.Value)
                    break;

                if (!lpYYToken->tkToken.tkData.lpSym->stFlags.Imm)
                {
                    // stData is a valid HGLOBAL variable or function array
                    Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.lpSym->stData.stGlbData)
                         || IsGlbTypeFcnDir (lpYYToken->tkToken.tkData.lpSym->stData.stGlbData));
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
                        dprintf ("**Zapping in FreeStrand: %08X (%d)",
                                 ClrPtrTypeDir (lpYYToken->tkToken.tkData.tkGlbData),
                                 __LINE__);
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
            case TKT_JOTDOT:
            case TKT_LPAREN:
            case TKT_RPAREN:
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR
} // End FreeStrand


//***************************************************************************
//  MakeVarStrand_EM
//
//  Make the variable strand into an immediate token or a global memory array.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeVarStrand_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeVarStrand_EM
    (LPYYSTYPE lpYYArg)         // Ptr to incoming token

{
    int         iLen, iBitIndex;
    APLUINT     ByteRes;
    LPYYSTYPE   lpYYToken, lpYYStrand;
    HGLOBAL     hGlbStr,
                hGlbRes;
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

    char    cStrandCurType = STRAND_INIT,
            cStrandNxtType,
            aplType;
    BOOL    bRet = TRUE;
    UINT YYLclIndex;

    DBGENTER;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // The strand needs to be saved to global memory

    // Save the base of this strand
    lpYYStrand                     =
    YYRes[YYLclIndex].unYYSTYPE.lpYYStrandBase = lpYYArg->unYYSTYPE.lpYYStrandBase;

    // Get the # elements in the strand
    iLen = gplLocalVars.lpYYStrandNext - lpYYStrand;

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
                if (!lpYYToken->tkToken.tkData.lpSym->stFlags.Value)
                {
                    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME, &lpYYToken->tkToken);

                    // Mark as in error
                    bRet = FALSE;

                    goto ERROR_EXIT;
                } // End IF

                if (!lpYYToken->tkToken.tkData.lpSym->stFlags.Imm)
                {
                    // stData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.lpSym->stData.stGlbData));

                    // Lock the memory to get a ptr to it
                    lpMemStr = MyGlobalLock (ClrPtrTypeDirGlb (lpYYToken->tkToken.tkData.lpSym->stData.stGlbData));

#define lpHeader    ((LPVARARRAY_HEADER) lpMemStr)

                    if (lpHeader->Rank EQ 0)
                        cStrandNxtType = TranslateArrayTypeToStrandType (lpHeader->ArrType);
                    else
                        cStrandNxtType = STRAND_NESTED;
#undef  lpHeader

                    MyGlobalUnlock (ClrPtrTypeDirGlb (lpYYToken->tkToken.tkData.lpSym->stData.stGlbData)); lpMemStr = NULL;

                    break;
                } // End IF

                // Handle the immediate case

                // tkData is LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

                // stData is an immediate
                Assert (lpYYToken->tkToken.tkData.lpSym->stFlags.Imm);

                switch (lpYYToken->tkToken.tkData.lpSym->stFlags.ImmType)
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

            case TKT_LIST:
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME, &lpYYArg->tkToken);

                // Mark as in error
                bRet = FALSE;

                goto ERROR_EXIT;

            case TKT_ASSIGN:
            case TKT_LISTSEP:
            case TKT_FCNIMMED:
            case TKT_COMMENT:
            case TKT_OP1IMMED:
            case TKT_OP2IMMED:
            case TKT_JOTDOT:
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
                        YYRes[YYLclIndex] = *lpYYStrand;

                        // If it's not an immediate, ...
                        if (!YYRes[YYLclIndex].tkToken.tkData.lpSym->stFlags.Imm)
                        {
                            // stData is a valid HGLOBAL variable array
                            Assert (IsGlbTypeVarDir (YYRes[YYLclIndex].tkToken.tkData.lpSym->stData.stGlbData));
                        } // End IF

                        break;

                    case TKT_VARIMMED:
                        // Pass through the entire token
                        YYRes[YYLclIndex] = *lpYYStrand;

                        break;

                    case TKT_STRING:
                        // Fill in the result token
                        YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
                        YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
                        YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
                    ////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
                        YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = lpYYStrand->tkToken.tkData.tkGlbData;
                        YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

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
                        Assert (!lpYYStrand->tkToken.tkData.lpSym->stFlags.Imm);

                        // stData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (lpYYStrand->tkToken.tkData.lpSym->stData.stGlbData));

                        // Pass through the entire token
                        YYRes[YYLclIndex] = *lpYYStrand;

                        break;

                    case TKT_VARARRAY:
                        // tkData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (lpYYStrand->tkToken.tkData.tkGlbData));

                        // Fill in the result token
                        YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
                        YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
                        YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
                    ////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
                        YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = lpYYStrand->tkToken.tkData.tkGlbData;
                        YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

                        // Mark as reused
                        lpYYStrand->tkToken.tkData.tkGlbData = PTR_REUSED;

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            case STRAND_STRING:     // e.g., 'abc'
                // Fill in the result token
                YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
                YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
                YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
            ////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
                YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = lpYYStrand->tkToken.tkData.tkGlbData;
                YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

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

    // Calculate the # bytes we'll need for the elements
    ByteRes = CalcArraySize (aplType, iLen, 1);
////switch (cStrandCurType)
////{
////    case STRAND_BOOL:
////        iByteCnt = sizeof (APLBOOL)   * (int) RoundUpBits8 (iLen);// One bit per bit
////
////        break;
////
////    case STRAND_INT:
////        iByteCnt = sizeof (APLINT)    * iLen;   // One number per APLINT
////
////        break;
////
////    case STRAND_CHAR:
////    case STRAND_CHARST:
////        iByteCnt = sizeof (APLCHAR)   * iLen;   // One char per APLCHAR
////
////        break;
////
////    case STRAND_FLOAT:
////        iByteCnt = sizeof (APLFLOAT)  * iLen;   // One number per APLFLOAT
////
////        break;
////
////    case STRAND_HETERO:
////        iByteCnt = sizeof (APLHETERO) * iLen;   // One value per APLHETERO
////
////        break;
////
////    case STRAND_NESTED:
////        iByteCnt = sizeof (APLNESTED) * iLen;   // One value per APLNESTED
////
////        break;
////
////    case STRAND_STRING:         // Handled by "if (iLen EQ 1)" code
////    defstop
////        return NULL;
////} // End SWITCH
////
////// Calculate the # bytes we'll need for the header
////iByteCnt += sizeof (VARARRAY_HEADER)// For header (including rank)
////          + sizeof (APLDIM) * 1;    // For the dimensions

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
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbStr);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

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
                        if (lpYYToken->tkToken.tkData.lpSym->stFlags.Imm)
                        {
                            // Copy the Boolean value to the result
                            *LPAPL.Bool |= (lpYYToken->tkToken.tkData.lpSym->stData.stBoolean) << iBitIndex++;
                        } else
                        {
                            // stData is an HGLOBAL
                            Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpSym->stData.lpVoid) EQ PTRTYPE_HGLOBAL);

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
                    if (lpYYToken->tkToken.tkData.lpSym->stFlags.Imm)
                    {
                        // Copy the integer value to the result
                        *LPAPL.Int++ = lpYYToken->tkToken.tkData.lpSym->stData.stInteger;
                    } else
                    {
                        // stData is an HGLOBAL
                        Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpSym->stData.lpVoid) EQ PTRTYPE_HGLOBAL);

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
                    if (lpYYToken->tkToken.tkData.lpSym->stFlags.Imm)
                    {
                        // Copy the char value to the result
                        *LPAPL.Char++ = lpYYToken->tkToken.tkData.lpSym->stData.stChar;
                    } else
                    {
                        // stData is an HGLOBAL
                        Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.lpSym->stData.lpVoid) EQ PTRTYPE_HGLOBAL);

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
                    if (lpYYToken->tkToken.tkData.lpSym->stFlags.Imm)
                    {
                        // Split cases based upon the symbol table entry's immediate type
                        switch (lpYYToken->tkToken.tkData.lpSym->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:
                                // Promote and copy the Boolean value to the result
                                *LPAPL.Float++ = (APLFLOAT) (lpYYToken->tkToken.tkData.lpSym->stData.stBoolean);

                                break;

                            case IMMTYPE_INT:
                                // ***FIXME*** -- Possible loss of precision

                                // Promote and copy the integer value to the result
                                *LPAPL.Float++ = (APLFLOAT) (lpYYToken->tkToken.tkData.lpSym->stData.stInteger);

                                break;

                            case IMMTYPE_FLOAT:
                                // Copy the float value to the result
                                *LPAPL.Float++ = lpYYToken->tkToken.tkData.lpSym->stData.stFloat;

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

        case STRAND_HETERO:     // lpYYToken->tkToken.tkData.lpSym (LPSYMENTRY)
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
                        if (lpYYToken->tkToken.tkData.lpSym->stFlags.Imm)
                        {
                            // Make a copy of the symbol table entry as we can't use the
                            //   one in the STNAME.
                            lpSym = CopyImmSymEntry_EM (lpYYToken->tkToken.tkData.lpSym,
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
                            Assert (IsGlbTypeVarDir (lpYYToken->tkToken.tkData.lpSym->stData.stGlbData));

                            hGlbRes = CopyArray_EM (ClrPtrTypeDirGlb (lpYYToken->tkToken.tkData.lpSym->stData.stGlbData),
                                                    FALSE,
                                                    &lpYYToken->tkToken);
                            if (hGlbRes)
                                *LPAPL.Nested++ = MakeGlbTypeGlb (hGlbRes);
                            else
                                bRet =FALSE;
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
    FreeStrand (gplLocalVars.lpYYStrandNext, gplLocalVars.lpYYStrandBase);

    // Strip the tokens on this portion of the strand stack
    StripStrand (&YYRes[YYLclIndex]);

    DBGLEAVE;

    return &YYRes[YYLclIndex];

ERROR_EXIT:
    // Free the entire strand stack
    FreeStrand (gplLocalVars.lpYYStrandNext, gplLocalVars.lpYYStrandStart);

    DBGLEAVE;

    return NULL;
} // End MakeVarStrand_EM
#undef  APPEND_NAME


//***************************************************************************
//  MakeFcnStrand_EM
//
//  Make the function strand into an immediate token or a global memory array.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeFcnStrand_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeFcnStrand_EM
    (LPYYSTYPE lpYYArg,         // Ptr to incoming token
     FCN_TYPES cFcnType)        // Type of the strand

{
    int            iLen, FcnCount = 0;
    APLUINT        ByteRes;
    LPYYSTYPE      lpYYStrand;
    HGLOBAL        hGlbStr;
    LPVOID         lpMemStr;
    LPYYSTYPE      lpYYMemStart, lpYYMemData, lpYYBase = (LPYYSTYPE) -1;
    BOOL           bRet = TRUE;
    UINT YYLclIndex;

    DBGENTER;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Zero the static YYRes
    ZeroMemory (&YYRes[YYLclIndex], sizeof (YYRes[YYLclIndex]));

    // Save the base of this strand
    lpYYStrand                     =
    YYRes[YYLclIndex].unYYSTYPE.lpYYStrandBase = lpYYArg->unYYSTYPE.lpYYStrandBase;

    // Get the (maximum) # elements in the strand
    iLen = gplLocalVars.lpYYStrandNext - lpYYStrand;

    //***********************************************************************
    //********** Single Element Case ****************************************
    //***********************************************************************

    // If there is a single element, pass through the entire token
    if (iLen EQ 1)
    {
        // Copy the entire token
        YYRes[YYLclIndex] = *CopyYYSTYPE_EM (lpYYArg->lpYYFcn, FALSE);
        YYRes[YYLclIndex].FcnCount = 1;

        lpYYBase = lpYYArg->lpYYFcn;

        goto NORMAL_EXIT;
    } // End IF

    //***********************************************************************
    //********** Multiple Element Case **************************************
    //***********************************************************************

    // In some cases, the following count (iLen) might be larger than needed
    //   because it may count some elements of the result twice because of
    //   calls to PushFcnStrand on the same argument (perhaps once for PRIMFCN,
    //   and later on once for LeftFunc).  The overcount is harmless and ignored.

    // Calculate the # bytes we'll need for the header and data
    ByteRes = sizeof (FCNARRAY_HEADER) // For the header
            + sizeof (YYSTYPE) * iLen; // For the data

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
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_FCNARRAY;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbStr);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;

    // Lock the global memory to get a ptr to it
    lpMemStr = MyGlobalLock (hGlbStr);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemStr)

    // Fill in the header
    lpHeader->Sign.ature = FCNARRAY_HEADER_SIGNATURE;
    lpHeader->FcnType    = cFcnType;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = iLen;

#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpYYMemStart = lpYYMemData = FcnArrayBaseToData (lpMemStr);

    // Copy the YYSTYPEs to the global memory object
    lpYYMemData = CopyYYFcn (lpYYMemData, lpYYArg->lpYYFcn, &lpYYBase, &FcnCount);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemStr)

    lpHeader->NELM = iLen = lpYYMemData - lpYYMemStart;

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbStr); lpMemStr = lpYYMemData = lpYYMemStart = NULL;

    // Handle case where we overallocated to the extent that there
    //   is only one function which can then be immediate.
    if (iLen EQ 1)
    {
        // Copy the entire token
        CopyYYFcn (&YYRes[YYLclIndex], lpYYArg->lpYYFcn, &lpYYBase, &FcnCount);
        Assert (FcnCount EQ 1);
        YYRes[YYLclIndex].FcnCount = FcnCount;

        // We no longer need this storage
        FreeResultGlobalFcn (hGlbStr); hGlbStr = NULL;
    } // End IF
NORMAL_EXIT:
    YYRes[YYLclIndex].unYYSTYPE.lpYYStrandBase  = gplLocalVars.lpYYStrandBase = lpYYBase;

#ifdef DEBUG
    DisplayStrand ();
#endif

    // Free the tokens on this portion of the strand stack
    FreeStrand (gplLocalVars.lpYYStrandNext, gplLocalVars.lpYYStrandBase);

    // Strip the tokens on this portion of the strand stack
    StripStrand (&YYRes[YYLclIndex]);

    DBGLEAVE;

    return &YYRes[YYLclIndex];

ERROR_EXIT:
    // Free the entire strand stack
    FreeStrand (gplLocalVars.lpYYStrandNext, gplLocalVars.lpYYStrandStart);

    DBGLEAVE;

    return NULL;
} // End MakeFcnStrand_EM
#undef  APPEND_NAME


//***************************************************************************
//  CopyYYFcn
//
//  Copy one or more YYSTYPE functions to a memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyYYFcn"
#else
#define APPEND_NAME
#endif

LPYYSTYPE CopyYYFcn
    (LPYYSTYPE  lpYYMem,
     LPYYSTYPE  lpYYArg,
     LPYYSTYPE *lpYYBase,
     LPINT      lpFcnCount)

{
    int       i, iLen, FcnCount, TotalFcnCount = 0;
    YYSTYPE   YYFcn = {0};
    HGLOBAL   hGlbData;
    LPTOKEN   lpToken;
    LPYYSTYPE lpYYMem0;

    iLen = lpYYArg->TknCount;

    // We need to modify the function count in the first token,
    //   so save its address in the array.
    lpYYMem0 = lpYYMem;

    // Loop through the tokens associated with this symbol
    for (i = 0; i < iLen; i++)
    {
        lpToken = &lpYYArg[i].tkToken;

        // Calculate the earliest function base
        *lpYYBase = min (*lpYYBase, lpYYArg[i].lpYYFcn);

        // Special case for named functions/operators
        if (lpToken->tkFlags.TknType EQ TKT_FCNNAMED
         || lpToken->tkFlags.TknType EQ TKT_OP1NAMED
         || lpToken->tkFlags.TknType EQ TKT_OP2NAMED)
        {
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's an immediate function/operator, copy it directly
            if (lpToken->tkData.lpSym->stFlags.Imm)
            {
                YYFcn = lpYYArg[i];
                YYFcn.tkToken.tkFlags.TknType   = TranslateImmTypeToTknType (lpToken->tkData.lpSym->stFlags.ImmType);
                YYFcn.tkToken.tkFlags.ImmType   = lpToken->tkData.lpSym->stFlags.ImmType;
////////////////YYFcn.tkToken.tkFlags.NoDisplay = 0;
////////////////YYFcn.tkToken.tkFlags.Color     = ??
                YYFcn.tkToken.tkData.tkLongest  = 0;        // Keep the extraneous data clear
                YYFcn.tkToken.tkData.tkChar     = lpToken->tkData.lpSym->stData.stChar;
////////////////YYFcn.tkToken.tkCharIndex       =
////////////////YYFcn.TknCount                  = lpYYArg[i].TknCount;
////////////////YYFcn.FcnCount                  = FcnCount;
////////////////YYFcn.Indirect                  =
////////////////YYFcn.lpYYFcn                   =
////////////////YYFcn.unYYSTYPE.lpYYStrandBase  =
            } else
            {
                hGlbData = lpToken->tkData.lpSym->stData.stGlbData;

                //stData is a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir (hGlbData));

                // Increment the reference count in global memory
                DbgIncrRefCntDir (hGlbData);

                YYFcn = lpYYArg[i];
                YYFcn.tkToken.tkFlags.TknType   = TKT_FCNARRAY;
////////////////YYFcn.tkToken.tkFlags.ImmType   = 0;
////////////////YYFcn.tkToken.tkFlags.NoDisplay = 0;
////////////////YYFcn.tkToken.tkFlags.Color     = ??
                YYFcn.tkToken.tkData.tkGlbData  = hGlbData;
////////////////YYFcn.tkToken.tkCharIndex       =
////////////////YYFcn.TknCount                  = lpYYArg[i].TknCount;
////////////////YYFcn.FcnCount                  = FcnCount;
////////////////YYFcn.Indirect                  =
////////////////YYFcn.lpYYFcn                   =
////////////////YYFcn.unYYSTYPE.lpYYStrandBase  =
            } // End IF/ELSE

            FcnCount = 1;
            YYFcn.TknCount = lpYYArg[i].TknCount;
            YYFcn.FcnCount = FcnCount;

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
            *lpYYMem++ = *CopyYYSTYPE_EM (&lpYYArg[i], FALSE);

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
//  TranslateFcnTypeToTknType
//
//  Translate a function type (see FCN_TYPES enum) to
//    a named token type (see TOKEN_TYPES enum).
//***************************************************************************

STRAND_TYPES TranslateFcnTypeToTknTypeNamed
    (FCN_TYPES fcnType)

{
    switch (fcnType)
    {
        case FCNTYPE_FCN:
        case FCNTYPE_AXISFCN:
            return TKT_FCNNAMED;

        case FCNTYPE_OP1:
            return TKT_OP1NAMED;

        case FCNTYPE_OP2:
            return TKT_OP2NAMED;

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateFcnTypeToTknTypeNamed


//***************************************************************************
//  TranslateImmTypeToTknType
//
//  Translate an immediate type (see IMM_TYPES enum) to
//    a token type (see TOKEN_TYPES enum).
//***************************************************************************

STRAND_TYPES TranslateImmTypeToTknType
    (IMM_TYPES immType)

{
    switch (immType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
        case IMMTYPE_CHAR:
        case IMMTYPE_FLOAT:
            return TKT_VARIMMED;

        case IMMTYPE_PRIMFCN:
            return TKT_FCNIMMED;

        case IMMTYPE_PRIMOP1:
            return TKT_OP1IMMED;

        case IMMTYPE_PRIMOP2:
            return TKT_OP2IMMED;

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateImmTypeToTknType


//***************************************************************************
//  TranslateImmTypeToStrandType
//
//  Translate an immediate type (see IMM_TYPES enum) to
//    a strand type (see STRAND_TYPES enum).
//***************************************************************************

STRAND_TYPES TranslateImmTypeToStrandType
    (IMM_TYPES immType)

{
    switch (immType)
    {
        case IMMTYPE_BOOL:
            return STRAND_BOOL;

        case IMMTYPE_INT:
            return STRAND_INT;

        case IMMTYPE_CHAR:
            return STRAND_CHAR;

        case IMMTYPE_FLOAT:
            return STRAND_FLOAT;

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateImmTypeToStrandType


//***************************************************************************
//  TranslateImmTypeToArrayType
//
//  Translate an immediate type (see IMM_TYPES enum) to
//    an array type (see ARRAY_TYPES enum).
//***************************************************************************

APLSTYPE TranslateImmTypeToArrayType
    (IMM_TYPES immType)

{
    switch (immType)
    {
        case IMMTYPE_BOOL:
            return ARRAY_BOOL;

        case IMMTYPE_INT:
            return ARRAY_INT;

        case IMMTYPE_CHAR:
            return ARRAY_CHAR;

        case IMMTYPE_FLOAT:
            return ARRAY_FLOAT;

        defstop
            return (APLSTYPE) -1;   // To keep the compiler happy
    } // End SWITCH
} // End TranslateImmTypeToArrayType


//***************************************************************************
//  TranslateArrayTypeToImmType
//
//  Translate an array type (see ARRAY_TYPES enum) to
//    an immediate type (see IMM_TYPES enum).
//***************************************************************************

IMM_TYPES TranslateArrayTypeToImmType
    (ARRAY_TYPES arrayType)

{
    switch (arrayType)
    {
        case ARRAY_BOOL:
            return IMMTYPE_BOOL;

        case ARRAY_INT:
        case ARRAY_APA:
            return IMMTYPE_INT;

        case ARRAY_FLOAT:
            return IMMTYPE_FLOAT;

        case ARRAY_CHAR:
            return IMMTYPE_CHAR;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
        case ARRAY_LIST:
        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateArrayTypeToImmType


//***************************************************************************
//  TranslateArrayTypeToStrandType
//
//  Translate an array type (see ARRAY_TYPES enum) to
//    a strand type (see STRAND_TYPES enum).
//***************************************************************************

STRAND_TYPES TranslateArrayTypeToStrandType
    (ARRAY_TYPES arrayType)

{
    switch (arrayType)
    {
        case ARRAY_BOOL:
            return STRAND_BOOL;

        case ARRAY_INT:
            return STRAND_INT;

        case ARRAY_FLOAT:
            return STRAND_FLOAT;

        case ARRAY_CHAR:
            return STRAND_CHAR;

        case ARRAY_HETERO:
            return STRAND_HETERO;

        case ARRAY_NESTED:
            return STRAND_NESTED;

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateArrayTypeToStrandType


//***************************************************************************
//  TranslateStrandTypeToArrayType
//
//  Translate a strand type (see STRAND_TYPES enum) to
//    an array type (see ARRAY_TYPES enum).
//***************************************************************************

ARRAY_TYPES TranslateStrandTypeToArrayType
    (STRAND_TYPES strandType)

{
    switch (strandType)
    {
        case STRAND_BOOL:
            return ARRAY_BOOL;

        case STRAND_INT:
            return ARRAY_INT;

        case STRAND_FLOAT:
            return ARRAY_FLOAT;

        case STRAND_CHAR:
        case STRAND_CHARST:
        case STRAND_STRING:
            return ARRAY_CHAR;

        case STRAND_HETERO:
            return ARRAY_HETERO;

        case STRAND_NESTED:
            return ARRAY_NESTED;

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateStrandTypeToArrayType


//***************************************************************************
//  ErrorMessageIndirectToken
//
//  Signal an error message, indirectly
//    and set the error token
//***************************************************************************

void ErrorMessageIndirectToken
    (WCHAR   *lpwszMsg,
     LPTOKEN lpError)

{
    // Save in global for later reference
    lpwszErrorMessage = lpwszMsg;

    // Set the error token
    ErrorMessageSetToken (lpError);
} // End ErrorMessageIndirectToken


//***************************************************************************
//  ErrorMessageSetToken
//
//  Set the error token for an error message
//***************************************************************************

void ErrorMessageSetToken
    (LPTOKEN lpError)

{
    // Set the error token
    gplLocalVars.tkErrorCharIndex = lpError->tkCharIndex;
} // End ErrorMessageSetToken


//***************************************************************************
//  CopyString_EM
//
//  Copy a string value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyString_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE CopyString_EM
    (LPYYSTYPE lpYYStr)

{
    HGLOBAL hGlbRes;
    UINT YYLclIndex;

    DBGENTER;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Zero the static YYRes
    ZeroMemory (&YYRes[YYLclIndex], sizeof (YYRes[YYLclIndex]));

    // tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (lpYYStr->tkToken.tkData.tkGlbData));

    hGlbRes = CopyArray_EM (ClrPtrTypeDirGlb (lpYYStr->tkToken.tkData.tkGlbData),
                            FALSE,
                            &lpYYStr->tkToken);
////if (!hGlbRes)
////{
////    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
////                               &lpYYStr->tkToken);
////    return NULL;
////} // End IF

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYStr->tkToken.tkCharIndex;

    DBGLEAVE;

    return &YYRes[YYLclIndex];
} // End CopyString_EM
#undef  APPEND_NAME


//***************************************************************************
//  MakeAxis
//
//  Make an axis value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeAxis"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeAxis
    (LPYYSTYPE lpYYAxis)

{
    UINT YYLclIndex;

    DBGENTER;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Split cases based upon the token type
    switch (lpYYAxis->tkToken.tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYAxis->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate, ...
            if (!lpYYAxis->tkToken.tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lpYYAxis->tkToken.tkData.lpSym->stData.stGlbData));

                // Fill in the result token
                YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_AXISARRAY;
                YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
                YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////////////////YYRes[YYLclIndex].tkToken.tkFlags.Color     = 0;
                YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = CopyArray_EM (lpYYAxis->tkToken.tkData.lpSym->stData.stGlbData, FALSE, NULL);
                YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYAxis->tkToken.tkCharIndex;

                break;
            } // End IF

            // Fill in the result token
            YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_AXISIMMED;
            YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = lpYYAxis->tkToken.tkData.lpSym->stFlags.ImmType;
            YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////////////YYRes[YYLclIndex].tkToken.tkFlags.Color     = 0;
            YYRes[YYLclIndex].tkToken.tkData.tkLongest  = lpYYAxis->tkToken.tkData.lpSym->stData.stLongest;
            YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYAxis->tkToken.tkCharIndex;

            break;

        case TKT_VARIMMED:
            // Copy the token and rename it
            YYRes[YYLclIndex] = *lpYYAxis;      // No need to CopyYYSTYPE_EM immediates
            YYRes[YYLclIndex].tkToken.tkFlags.TknType = TKT_AXISIMMED;

            break;

        case TKT_VARARRAY:
            // Copy the token and rename it
            YYRes[YYLclIndex] = *CopyYYSTYPE_EM (lpYYAxis, FALSE);
            YYRes[YYLclIndex].tkToken.tkFlags.TknType = TKT_AXISARRAY;

            break;

        defstop
            break;
    } // End SWITCH

    DBGLEAVE;

    return &YYRes[YYLclIndex];
} // End MakeAxis
#undef  APPEND_NAME


//***************************************************************************
//  MakePrimFcn
//
//  Make a token a primitive function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePrimFcn"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakePrimFcn
    (LPYYSTYPE lpYYFcn)

{
    UINT YYLclIndex;

    DBGENTER;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    YYRes[YYLclIndex] = *CopyYYSTYPE_EM (lpYYFcn, FALSE);
    YYRes[YYLclIndex].tkToken.tkFlags.TknType = TKT_FCNIMMED;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
    YYRes[YYLclIndex].lpYYFcn = NULL;

    DBGLEAVE;

    return &YYRes[YYLclIndex];
} // End MakePrimFcn
#undef  APPEND_NAME


//***************************************************************************
//  MakeNameFcn
//
//  Make a token for a named function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeNameFcn"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeNameFcn
    (LPYYSTYPE lpYYFcn)

{
    UINT YYLclIndex;

    DBGENTER;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Because when the tokens are first created we don't
    //   know the type of a name, it is arbitrarily typed
    //   as TKT_VARNAMED.  Now that we know it's a function,
    //   change its type to TKT_FCNNAMED.

    lpYYFcn->tkToken.tkFlags.TknType = TKT_FCNNAMED;
    YYRes[YYLclIndex] = *CopyYYSTYPE_EM (lpYYFcn, FALSE);
    YYRes[YYLclIndex].lpYYFcn = NULL;

    DBGLEAVE;

    return &YYRes[YYLclIndex];
} // End MakeNameFcn
#undef  APPEND_NAME


//***************************************************************************
//  MakeOp1
//
//  Make a monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeOp1"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeOp1
    (LPYYSTYPE lpYYOp1)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    YYRes[YYLclIndex] = *lpYYOp1;           // No need to CopyYYSTYPE_EM immediates
    YYRes[YYLclIndex].tkToken.tkFlags.TknType = TKT_OP1IMMED;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP1;
    YYRes[YYLclIndex].lpYYFcn = NULL;

    return &YYRes[YYLclIndex];
} // End MakeOp1
#undef  APPEND_NAME


//***************************************************************************
//  MakeNameOp1
//
//  Make a named monadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeNameOp1"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeNameOp1
    (LPYYSTYPE lpYYOp1)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Because when the tokens are first created we don't
    //   know the type of a name, it is arbitrarily typed
    //   as TKT_VARNAMED.  Now that we know it's a monadic operator,
    //   change its type to TKT_OP1NAMED.

    lpYYOp1->tkToken.tkFlags.TknType = TKT_OP1NAMED;

    YYRes[YYLclIndex] = *CopyYYSTYPE_EM (lpYYOp1, FALSE);
    YYRes[YYLclIndex].tkToken.tkFlags.TknType = TKT_OP1NAMED;
    YYRes[YYLclIndex].lpYYFcn = NULL;

    return &YYRes[YYLclIndex];
} // End MakeNameOp1
#undef  APPEND_NAME


//***************************************************************************
//  MakeOp2
//
//  Make a dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeOp2"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeOp2
    (LPYYSTYPE lpYYOp2)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    YYRes[YYLclIndex] = *lpYYOp2;           // No need to CopyYYSTYPE_EM immediates
    YYRes[YYLclIndex].tkToken.tkFlags.TknType = TKT_OP2IMMED;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP2;
    YYRes[YYLclIndex].lpYYFcn = NULL;

    return &YYRes[YYLclIndex];
} // End MakeOp2
#undef  APPEND_NAME


//***************************************************************************
//  MakeNameOp2
//
//  Make a named dyadic operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeNameOp2"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeNameOp2
    (LPYYSTYPE lpYYOp2)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Because when the tokens are first created we don't
    //   know the type of a name, it is arbitrarily typed
    //   as TKT_VARNAMED.  Now that we know it's a dyadic operator,
    //   change its type to TKT_OP2NAMED.

    lpYYOp2->tkToken.tkFlags.TknType = TKT_OP2NAMED;

    YYRes[YYLclIndex] = *lpYYOp2;
    YYRes[YYLclIndex].tkToken.tkFlags.TknType = TKT_OP2NAMED;
    YYRes[YYLclIndex].lpYYFcn = NULL;

    return &YYRes[YYLclIndex];
} // End MakeNameOp2
#undef  APPEND_NAME


//***************************************************************************
//  InitNameStrand
//
//  Initialize a name strand
//***************************************************************************

void InitNameStrand
    (LPYYSTYPE lpYYArg)         // Ptr to the incoming argument

{
    // Set the base of this strand to the next available location
    lpYYArg->unYYSTYPE.lpYYStrandBase =
    gplLocalVars.lpYYStrandBase       = gplLocalVars.lpYYStrandNext;
} // End InitNameStrand


//***************************************************************************
//  PushNameStrand
//
//  Push a name strand
//***************************************************************************

LPYYSTYPE PushNameStrand
    (LPYYSTYPE lpYYArg)         // Ptr to the incoming argument

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_STRAND;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.lpVoid     = (LPVOID) -1;
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;

    YYRes[YYLclIndex].unYYSTYPE.lpYYStrandBase  =
    lpYYArg->unYYSTYPE.lpYYStrandBase = gplLocalVars.lpYYStrandBase;

    // Save this token on the strand stack
    //   and skip over it
    *gplLocalVars.lpYYStrandNext++ = *lpYYArg;

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand ();
#endif

    return &YYRes[YYLclIndex];
} // End PushNameStrand


//***************************************************************************
//  MakeNameStrand_EM
//
//  Make a name strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeNameStrand_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeNameStrand_EM
    (LPYYSTYPE lpYYArg)         // Ptr to incoming token

{
    int         iLen;
    APLUINT     ByteRes;
    LPYYSTYPE   lpYYStrand;
    HGLOBAL     hGlbStr;
    LPVOID      lpMemStr;
    BOOL        bRet = TRUE;
    UINT YYLclIndex;

    DBGENTER;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Save the base of this strand
    lpYYStrand                     =
    YYRes[YYLclIndex].unYYSTYPE.lpYYStrandBase = lpYYArg->unYYSTYPE.lpYYStrandBase;

    // Get the # elements in the strand
    iLen = gplLocalVars.lpYYStrandNext - lpYYStrand;

    // Save these tokens in global memory

    // Calculate storage needed for the tokens
    ByteRes = sizeof (VARNAMED_HEADER)
            + sizeof (lpYYStrand[0]) * iLen;

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
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_STRNAMED;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbStr);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

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
    FreeStrand (gplLocalVars.lpYYStrandNext, gplLocalVars.lpYYStrandBase);

    // Strip the tokens on this portion of the strand stack
    StripStrand (&YYRes[YYLclIndex]);

    DBGLEAVE;

    return &YYRes[YYLclIndex];

ERROR_EXIT:
    // Free the entire strand stack
    FreeStrand (gplLocalVars.lpYYStrandNext, gplLocalVars.lpYYStrandStart);

    DBGLEAVE;

    return NULL;
} // End MakeNameStrand_EM
#undef  APPEND_NAME


//***************************************************************************
//  InitList0
//
//  Initialize a list starting with an empty token
//***************************************************************************

LPYYSTYPE InitList0
    (void)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_LIST;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.lpVoid     = (LPVOID) -1;
    YYRes[YYLclIndex].tkToken.tkCharIndex       = (UINT) -1 ;

    // Set the base of this strand to the next available location
    YYRes[YYLclIndex].unYYSTYPE.lpYYStrandBase  =
    gplLocalVars.lpYYStrandBase     = gplLocalVars.lpYYStrandNext;

    return PushList (&YYRes[YYLclIndex], NULL);
} // End InitList0


//***************************************************************************
//  InitList1
//
//  Initialize a list starting with a single token
//***************************************************************************

LPYYSTYPE InitList1
    (LPYYSTYPE lpYYArg)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_LIST;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.lpVoid     = (LPVOID) -1;
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYArg->tkToken.tkCharIndex;

    // Set the base of this strand to the next available location
    YYRes[YYLclIndex].unYYSTYPE.lpYYStrandBase  =
    gplLocalVars.lpYYStrandBase     = gplLocalVars.lpYYStrandNext;

    return PushList (&YYRes[YYLclIndex], lpYYArg);
} // End InitList1


//***************************************************************************
//  PushList
//
//  Push a token onto the list stack
//***************************************************************************

LPYYSTYPE PushList
    (LPYYSTYPE lpYYStrand,
     LPYYSTYPE lpYYArg)

{
    YYSTYPE YYTmp;
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // Fill in the result token
    YYRes[YYLclIndex] = *lpYYStrand;

    // If the token is NULL, push an empty token
    if (lpYYArg EQ NULL)
    {
        YYTmp.tkToken.tkFlags.TknType   = TKT_LISTSEP;
        YYTmp.tkToken.tkData.lpVoid     = (LPVOID) -1;
        YYTmp.tkToken.tkCharIndex       = (UINT) -1;
        YYTmp.unYYSTYPE.lpYYStrandBase  = gplLocalVars.lpYYStrandBase;
        lpYYArg = &YYTmp;
    } // End IF

    // Save this token on the strand stack
    //   and skip over it
    *gplLocalVars.lpYYStrandNext++ = *lpYYArg;

#ifdef DEBUG
    // Display the strand stack
    DisplayStrand ();
#endif

    return &YYRes[YYLclIndex];
} // End PushList


//***************************************************************************
//  MakeList_EM
//
//  Make the listy into a global memory array.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeList_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE MakeList_EM
    (LPYYSTYPE lpYYArg)         // Ptr to incoming token

{
    LPYYSTYPE  lpYYStrand,
               lpYYToken;
    int        iLen;
    APLUINT    ByteRes;
    HGLOBAL    hGlbLst;
    LPVOID     lpMemLst;
    LPSYMENTRY lpSymEntry;
    BOOL       bRet = TRUE;
    UINT YYLclIndex;

    DBGENTER;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    // The list needs to be saved to global memory

    // Save the base of this strand
    lpYYStrand                     =
    YYRes[YYLclIndex].unYYSTYPE.lpYYStrandBase = lpYYArg->unYYSTYPE.lpYYStrandBase;

    // Get the # elements in the strand
    iLen = gplLocalVars.lpYYStrandNext - lpYYStrand;

    // Calculate the # bytes we'll need for the header and the elements
    ByteRes = CalcArraySize (ARRAY_NESTED, iLen, 1);
////ByteRes = sizeof (VARARRAY_HEADER)      // For header (including rank)
////        + sizeof (APLDIM) * 1           // For the dimensions
////        + iLen * sizeof (APLNESTED);    // One value per APLNESTED

    // Allocate global memory for a length <iLen> vector of type <cState>
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbLst = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbLst)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   &lpYYArg->tkToken);
        DBGLEAVE;

        // Mark as in error
        return NULL;
    } // End IF

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_LIST;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbLst);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lpYYStrand->tkToken.tkCharIndex;

    // Lock the global memory
    lpMemLst = MyGlobalLock (hGlbLst);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemLst)

    // Fill in the header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_LIST;
////lpHeader->Perm       = 0;
////lpHeader->SysVar     = 0;
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
    StripStrand (&YYRes[YYLclIndex]);

    DBGLEAVE;

    return &YYRes[YYLclIndex];
} // End MakeList_EM
#undef  APPEND_NAME


//***************************************************************************
//  CopyImmToken_EM
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
//  CopyToken_EM
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
    static TOKEN tkRes;

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
            if (!lpToken->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lpToken->tkData.lpSym->stData.stGlbData));

                // Increment the reference count in global memory
                DbgIncrRefCntDir (lpToken->tkData.lpSym->stData.stGlbData);

                break;          // We're done
            } // End IF

            break;              // Ignore immediates

        case TKT_FCNNAMED:      // tkData is LPSYMENTRY
        case TKT_OP1NAMED:      // tkData is LPSYMENTRY
        case TKT_OP2NAMED:      // tkData is LPSYMENTRY
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lpToken->tkData.lpSym->stFlags.Imm)
            {
                STFLAGS stFlags;

                // Skip assertion if it's some kind of function
                stFlags = lpToken->tkData.lpSym->stFlags;
                if (stFlags.SysFn0
                 || stFlags.SysFn12
                 || stFlags.UsrFn0
                 || stFlags.UsrFn12
                 || stFlags.UsrOp1
                 || stFlags.UsrOp2)
                    break;

                // stData is a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir (lpToken->tkData.lpSym->stData.stGlbData));

                // Increment the reference count in global memory
                DbgIncrRefCntDir (lpToken->tkData.lpSym->stData.stGlbData);

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

        case TKT_LIST:          // tkData is HGLOBAL
            DbgBrk ();          // ***FINISHME***





            break;

        case TKT_VARIMMED:      // tkData is immediate
        case TKT_FCNIMMED:      // ...
        case TKT_AXISIMMED:     // ...
        case TKT_OP1IMMED:      // ...
        case TKT_OP2IMMED:      // ...
        case TKT_JOTDOT:        // ...
            break;              // Ignore immediates

        defstop
            break;
    } // End SWITCH

    DBGLEAVE;

    return &tkRes;
} // End CopyToken_EM
#undef  APPEND_NAME


//***************************************************************************
//  CopyYYSTYPE_EM
//
//  Make a copy of a YYSTYPE, incrementing ref count if not changing
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyYYSTYPE_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE CopyYYSTYPE_EM
    (LPYYSTYPE lpYYArg,
     BOOL      bChanging)   // TRUE iff we're going to change the HGLOBAL

{
    UINT YYLclIndex;

    DBGENTER;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

    Assert (bChanging EQ FALSE);

    // Copy the YYSTYPE
    YYRes[YYLclIndex] = *lpYYArg;

    // Make a copy of the token within
    YYRes[YYLclIndex].tkToken = *CopyToken_EM (&lpYYArg->tkToken, bChanging);

    DBGLEAVE;

    return &YYRes[YYLclIndex];
} // End CopyYYSTYPE_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: strand.c
//***************************************************************************
