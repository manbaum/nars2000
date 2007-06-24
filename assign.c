//***************************************************************************
//  NARS2000 -- Assignment Functions
//***************************************************************************

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
//  $AssignName_EM
//
//  Assign a token value to a token name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignName_EM"
#else
#define APPEND_NAME
#endif

BOOL AssignName_EM
    (LPTOKEN lptkName,              // Ptr to name token
     LPTOKEN lptkExpr)              // Ptr to value token

{
    STFLAGS stFlags = {0};
    HGLOBAL hGlbExpr;

    DBGENTER;

    // Because when the tokens are first created we don't
    //   know the type of a name, it is arbitrarily typed
    //   as TKT_VARNAMED.  Now is the time to set the record
    //   straight.
    SetNameType (lptkName);

    // It's a named variable or function
    Assert (lptkName->tkFlags.TknType EQ TKT_VARNAMED
         || lptkName->tkFlags.TknType EQ TKT_FCNNAMED
         || lptkName->tkFlags.TknType EQ TKT_FCNIMMED
         || lptkName->tkFlags.TknType EQ TKT_OP1NAMED
         || lptkName->tkFlags.TknType EQ TKT_OP2NAMED);

    // If the target is a system var, validate the assignment
    //   before we free the old value
    if (lptkName->tkData.tkSym->stFlags.SysVar)
    {
        // If the target is a defined function system label, signal a SYNTAX ERROR
        if (lptkName->tkData.tkSym->stFlags.DfnSysLabel)
        {
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkName);
            DBGLEAVE;
            return FALSE;
        } // End IF

        // Validate the value
        return (*aSysVarValid[lptkName->tkData.tkSym->stFlags.SysVarValid]) (lptkName, lptkExpr);
    } // End IF

    // Note that we have to wait until all errors have been
    //   resolved before calling FreeResultName.

    // Split cases based upon the expression token type
    switch (lptkExpr->tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkExpr->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If the target is a defined function label, signal a SYNTAX ERROR
            if (lptkName->tkData.tkSym->stFlags.DfnLabel)
            {
                ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                           lptkName);
                DBGLEAVE;
                return FALSE;
            } // End IF

            // If the expression is not immediate, ...
            if (!lptkExpr->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable or function array
                Assert (IsGlbTypeVarDir (lptkExpr->tkData.tkSym->stData.stGlbData)
                     || IsGlbTypeFcnDir (lptkExpr->tkData.tkSym->stData.stGlbData));

                // ***FIXME*** -- Do we need the tests below??
                //                Try []WSID {is} []SA
                //                and []WSID {is} []WSID

////////////////// There are no reference counts on .SysName variables
////////////////if (lptkName->tkData.tkSym->stFlags.SysVar
//////////////// || lptkExpr->tkData.tkSym->stFlags.SysVar)
////////////////{
                    hGlbExpr = CopySymGlbDir (lptkExpr->tkData.tkSym->stData.stGlbData);

                    // Free the old value for this name
                    FreeResultName (lptkName);

                    // Save the new global memory ptr
                    lptkName->tkData.tkSym->stData.stGlbData = hGlbExpr;

                    break;
////////////////} // End IF
////////////////
////////////////// If the global memory ptrs are different, there's no
//////////////////   need to copy the expression's ptr and it would be
//////////////////   a mistake to increment the reference count
////////////////if (lptkName->tkData.tkSym->stData.stGlbData NE
////////////////    lptkExpr->tkData.tkSym->stData.stGlbData)
////////////////{
////////////////    // Copy the global memory ptr
////////////////    lptkName->tkData.tkSym->stData.stGlbData =
////////////////    lptkExpr->tkData.tkSym->stData.stGlbData;
////////////////
////////////////    // Increment the reference count in global memory
////////////////    DbgIncrRefCntDir (lptkName->tkData.tkSym->stData.stGlbData);
////////////////} // End IF
////////////////
////////////////break;
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkExpr->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lptkExpr->tkData.tkSym->stFlags.Imm);

            // Free the old value for this name
            FreeResultName (lptkName);

            // Because the source and the destination
            //   may well have very different characteristics
            //   (SysVar vs. UsrVar), we can't re-use the
            //   source LPSYMENTRY, so we copy the values
            //   into the target's LPSYMENTRY.

            // Copy the expression's flags
            stFlags = lptkExpr->tkData.tkSym->stFlags;

            // Include the expression's .Imm & .ImmType flags
            lptkName->tkData.tkSym->stFlags.Imm      =
                                    stFlags.Imm;
            lptkName->tkData.tkSym->stFlags.ImmType  =
                                    stFlags.ImmType;
            // Copy the constant data
            lptkName->tkData.tkSym->stData.stLongest =
            lptkExpr->tkData.tkSym->stData.stLongest;

            break;

        case TKT_VARIMMED:
            // Free the old value for this name
            FreeResultName (lptkName);

            // Because the source and the destination
            //   may well have very different characteristics
            //   (SysVar vs. UsrVar), we can't re-use the
            //   source LPSYMENTRY, so we copy the values
            //   into the target's LPSYMENTRY.

            // It's an immediate
            lptkName->tkData.tkSym->stFlags.Imm = 1;

            // Include the expression's .ImmType flags
            lptkName->tkData.tkSym->stFlags.ImmType =
                          lptkExpr->tkFlags.ImmType;

            // Copy the constant data
            lptkName->tkData.tkSym->stData.stLongest=
                          lptkExpr->tkData.tkLongest;
            break;

        case TKT_FCNIMMED:
            // Free the old value for this name
            FreeResultName (lptkName);

            // It's an immediate primitive function
            lptkName->tkData.tkSym->stFlags.Imm     = 1;
            lptkName->tkData.tkSym->stFlags.ImmType = GetImmTypeFcn (lptkExpr->tkData.tkChar);
            lptkName->tkData.tkSym->stFlags.UsrFn12 = 1;

            // Copy the constant data
            lptkName->tkData.tkSym->stData.stLongest=
                          lptkExpr->tkData.tkLongest;
            break;

        case TKT_OP1IMMED:
            // Free the old value for this name
            FreeResultName (lptkName);

            // It's an immediate primitive operator
            lptkName->tkData.tkSym->stFlags.Imm     = 1;
            lptkName->tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMOP1;
            lptkName->tkData.tkSym->stFlags.UsrOp1  = 1;

            // Copy the constant data
            lptkName->tkData.tkSym->stData.stLongest=
                          lptkExpr->tkData.tkLongest;
            break;

        case TKT_OP2IMMED:
            // Free the old value for this name
            FreeResultName (lptkName);

            // It's an immediate primitive operator
            lptkName->tkData.tkSym->stFlags.Imm     = 1;
            lptkName->tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMOP2;
            lptkName->tkData.tkSym->stFlags.UsrOp2  = 1;

            // Copy the constant data
            lptkName->tkData.tkSym->stData.stLongest=
                          lptkExpr->tkData.tkLongest;
            break;

        case TKT_VARARRAY:
            AssignArrayCommon (lptkName, lptkExpr, TKT_VARNAMED);

            break;

        case TKT_FCNARRAY:
            AssignArrayCommon (lptkName, lptkExpr, TKT_FCNNAMED);

            break;

        defstop
            break;
    } // End SWITCH

    // If this is a named function, mark it as such
    if (lptkExpr->tkFlags.TknType EQ TKT_FCNNAMED
     || lptkExpr->tkFlags.TknType EQ TKT_FCNARRAY)
    {
        // Split cases based upon the underlying FCNTYPE_xxx
        switch (GetFcnType (lptkExpr))
        {
            case FCNTYPE_FCN:
            case FCNTYPE_AXISFCN:
                lptkName->tkData.tkSym->stFlags.UsrFn12 = 1;

                break;

            case FCNTYPE_OP1:
                lptkName->tkData.tkSym->stFlags.UsrOp1 = 1;

                break;

            case FCNTYPE_OP2:
                lptkName->tkData.tkSym->stFlags.UsrOp2 = 1;

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    // Mark as valued
    lptkName->tkData.tkSym->stFlags.Value = 1;

    // Ensure either .SysVar or .UsrVar is set
    if (lptkName->tkData.tkSym->stFlags.SysName
     && !lptkName->tkData.tkSym->stFlags.SysFn12)
        lptkName->tkData.tkSym->stFlags.SysVar = 1;
    else
    if (lptkName->tkData.tkSym->stFlags.UsrName
     && !lptkName->tkData.tkSym->stFlags.UsrFn12
     && !lptkName->tkData.tkSym->stFlags.UsrOp1
     && !lptkName->tkData.tkSym->stFlags.UsrOp2)
        lptkName->tkData.tkSym->stFlags.UsrVar = 1;

    // Mark as not displayable
    lptkName->tkFlags.NoDisplay = 1;

    Assert (HshTabFrisk ());

#ifdef DEBUG
    DisplayFcnStrand (lptkName);
#endif
    DBGLEAVE;

    return TRUE;
} // End AssignName_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SetNameType
//
//  Set the name type (TKT_xxx) for a named variable
//***************************************************************************

void SetNameType
    (LPTOKEN lpToken)

{
    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, ...
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // If it has no value, exit
                if (!lpToken->tkData.tkSym->stFlags.Value)
                    return;

                // stData is an HGLOBAL function array
                Assert (IsGlbTypeFcnDir (lpToken->tkData.tkSym->stData.stGlbData));

                // Set the new token type
                lpToken->tkFlags.TknType =
                  TranslateFcnTypeToTknTypeNamed (GetFcnType (lpToken));
            } else
                lpToken->tkFlags.TknType =
                  TranslateImmTypeToTknType (lpToken->tkData.tkSym->stFlags.ImmType);
            break;

        case TKT_VARNAMED:      // Nothing to do
        case TKT_OP1NAMED:      // ...
        case TKT_OP2NAMED:      // ...
            break;

        defstop
            break;
    } // End SWITCH
} // End SetNameType


//***************************************************************************
//  $GetFcnType
//
//  Return the FCNTYPE_xxx of a function token
//***************************************************************************

FCN_TYPES GetFcnType
    (LPTOKEN lpToken)

{
    HGLOBAL hGlbData;
    LPVOID  lpMem;
    FCN_TYPES cFcnType;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate, ...
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                break;      // Continue with common hGlbData code
            } // End IF

            // Split cases based upon the immediate token type
            switch (lpToken->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_PRIMFCN:
                    return FCNTYPE_FCN;

                case IMMTYPE_PRIMOP1:
                    return FCNTYPE_OP1;

                case IMMTYPE_PRIMOP2:
                    return FCNTYPE_OP2;

                defstop
                    break;
            } // End SWITCH

            return -1;

        case TKT_FCNARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            break;      // Continue with common hGlbData code

        defstop
            return -1;
    } // End SWITCH

    // stData/tkData is a valid HGLOBAL function array
    Assert (IsGlbTypeFcnDir (hGlbData));

    hGlbData = ClrPtrTypeDirGlb (hGlbData);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)

    cFcnType = lpHeader->FcnType;

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    return cFcnType;
} // End GetFcnType


//***************************************************************************
//  $AssignArrayCommon
//
//  Assign either a variable or function array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignArrayCommon_EM"
#else
#define APPEND_NAME
#endif

void AssignArrayCommon
    (LPTOKEN     lptkName,
     LPTOKEN     lptkExpr,
     TOKEN_TYPES TknType)

{
    // Free the old value for this name
    FreeResultName (lptkName);

    // Fill in the token type
    lptkName->tkFlags.TknType = TknType;

    // Copy the HGLOBAL
////lptkName->tkData.tkSym->stData.stNameFcn = ...
    lptkName->tkData.tkSym->stData.stGlbData = CopySymGlbDir (lptkExpr->tkData.tkGlbData);
} // End AssignArrayCommon
#undef  APPEND_NAME


//***************************************************************************
//  $AssignNameSpec_EM
//
//  Assign values to a name strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignNameSpec_EM"
#else
#define APPEND_NAME
#endif

BOOL AssignNameSpec_EM
    (LPTOKEN       lptkStr,         // Ptr to named strand token
     LPTOKEN       lptkVal)         // Ptr to value token

{
    BOOL     bRet = TRUE;
    HGLOBAL  hGlbStr,
             hGlbVal,
             hGlbSub;
    LPVOID   lpMemStr,
             lpMemVal;
    APLNELM  aplNELMStr,
             aplNELMVal,
             aplName;
    APLRANK  aplRankVal;
    APLSTYPE aplTypeVal;
    TOKEN    tkToken = {0};
    UINT     uBitMaskVal;
    LPSYMENTRY lpSymVal;
    APLINT   apaOffVal,
             apaMulVal;

    DBGENTER;

    Assert (lptkStr->tkFlags.TknType EQ TKT_STRNAMED);

    // Get the global handle
    hGlbStr = ClrPtrTypeDirGlb (lptkStr->tkData.tkGlbData);

    // Lock the memory to get a ptr to it
    lpMemStr = MyGlobalLock (hGlbStr);

#define lpHeader    ((LPVARNAMED_HEADER) lpMemStr)

    // Get the # names in the strand
    aplNELMStr = lpHeader->NELM;

#undef  lpHeader

    // Skip over the name strand header to the data
    lpMemStr = VarNamedBaseToData (lpMemStr);

    // Split cases based upon the value's token type
    switch (lptkVal->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkVal->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkVal->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory handle
                hGlbVal = lptkVal->tkData.tkSym->stData.stGlbData;

                break;          // Continue with global memory case
            } // End IF

            // Handle the immediate case

            // Fall through to common code

        case TKT_VARIMMED:
            // Assign this immediate value to each name
            for (aplName = 0; aplName < aplNELMStr; aplName++)
                AssignName_EM (&((LPYYSTYPE) lpMemStr)[aplName].tkToken, lptkVal);
            goto NORMAL_EXIT;

        case TKT_VARARRAY:
            // Get the global memory handle
            hGlbVal = lptkVal->tkData.tkGlbData;

            break;

        defstop
            break;
    } // End SWITCH

    // Handle global case

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbVal));

    // Clear the ptr type bits
    hGlbVal = ClrPtrTypeDirGlb (hGlbVal);

    // Lock the memory to get a ptr to it
    lpMemVal = MyGlobalLock (hGlbVal);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemVal)

    // Get the Type, NELM, and Rank
    aplTypeVal = lpHeader->ArrType;
    aplRankVal = lpHeader->Rank;
    aplNELMVal = lpHeader->NELM;

#undef  lpHeader

    // Check for RANK ERROR
    if (aplRankVal > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkVal);
        // Mark as in error
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Check for LENGTH ERROR
    if (aplNELMVal NE 1
     && aplNELMVal NE aplNELMStr)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkVal);
        // Mark as in error
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Skip over the header and dimension to the data
    lpMemVal = VarArrayBaseToData (lpMemVal, aplRankVal);

#define lpAPA       ((LPAPLAPA) lpMemVal)

    // If the value is an APA, get its parameters
    if (aplTypeVal EQ ARRAY_APA)
    {
        apaOffVal = lpAPA->Off;
        apaMulVal = lpAPA->Mul;
    } // End IF

#undef  lpAPA

    // Fill in the value token
////tkToken.tkFlags.TknType     =
////tkToken.tkFlags.ImmType     =
////tkToken.tkFlags.NoDisplay   = 0;    // Already from from {0}
////tkToken.tkData              =
    tkToken.tkCharIndex         = lptkVal->tkCharIndex;

    // Split cases based upon the value storage type
    switch (aplTypeVal)
    {
        case ARRAY_BOOL:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_BOOL;

            uBitMaskVal = 0x01;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMStr; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkBoolean = (uBitMaskVal & *(LPAPLBOOL) lpMemVal) ? 1 : 0;

                // Assign this token to this name
                AssignName_EM (&((LPYYSTYPE) lpMemStr)[(aplNELMStr - 1) - aplName].tkToken, &tkToken);

                // If there's more than one value, ...
                if (aplNELMVal NE 1)
                {
                    // Shift over the bit mask
                    uBitMaskVal <<= 1;

                    // Check for end-of-byte
                    if (uBitMaskVal EQ END_OF_BYTE)
                    {
                        uBitMaskVal = 0x01;         // Start over
                        ((LPAPLBOOL) lpMemVal)++;   // Skip to next byte
                    } // End IF
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_INT;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMStr; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkInteger = ((LPAPLINT) lpMemVal)[aplName % aplNELMVal];

                // Assign this token to this name
                AssignName_EM (&((LPYYSTYPE) lpMemStr)[(aplNELMStr - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_FLOAT:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_FLOAT;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMStr; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkFloat = ((LPAPLFLOAT) lpMemVal)[aplName % aplNELMVal];

                // Assign this token to this name
                AssignName_EM (&((LPYYSTYPE) lpMemStr)[(aplNELMStr - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_CHAR;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMStr; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkChar = ((LPAPLCHAR) lpMemVal)[aplName % aplNELMVal];

                // Assign this token to this name
                AssignName_EM (&((LPYYSTYPE) lpMemStr)[(aplNELMStr - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_HETERO:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMStr; aplName++)
            {
                // Get a ptr to the next value
                lpSymVal = ((LPAPLHETERO) lpMemVal)[aplName % aplNELMVal];

                // It's an LPSYMENTRY
                Assert (GetPtrTypeDir (lpSymVal) EQ PTRTYPE_STCONST);

                // It's an immediate
                Assert (lpSymVal->stFlags.Imm);

                // Save the immediate type and value in the token
                tkToken.tkFlags.ImmType = lpSymVal->stFlags.ImmType;
                tkToken.tkData.tkLongest = lpSymVal->stData.stLongest;

                // Assign this token to this name
                AssignName_EM (&((LPYYSTYPE) lpMemStr)[(aplNELMStr - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_NESTED:
            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMStr; aplName++)
            {
                LPVOID lpVal;

                // Get the LPSYMENTRTY or HGLOBAL
                lpVal = ((LPAPLNESTED) lpMemVal)[aplName % aplNELMVal];

                // Split cases based the ptr type of the value
                switch (GetPtrTypeDir (lpVal))
                {
                    case PTRTYPE_STCONST:
                        // Get the LPSYMENTRY
                        lpSymVal = (LPSYMENTRY) lpVal;

                        // It's an immediate
                        Assert (lpSymVal->stFlags.Imm);

                        // Fill in the value token
                        tkToken.tkFlags.TknType  = TKT_VARIMMED;
                        tkToken.tkFlags.ImmType  = lpSymVal->stFlags.ImmType;
                        tkToken.tkData.tkLongest = lpSymVal->stData.stLongest;

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Get the HGLOBAL
                        hGlbSub = (HGLOBAL) lpVal;

                        // It's a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (hGlbSub));

                        // Fill in the value token
                        tkToken.tkFlags.TknType  = TKT_VARARRAY;
                        tkToken.tkFlags.ImmType  = 0;
                        tkToken.tkData.tkGlbData = hGlbSub;     // The call to AssignName_EM increments the RefCnt

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Assign this token to this name
                AssignName_EM (&((LPYYSTYPE) lpMemStr)[(aplNELMStr - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_APA:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_INT;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMStr; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkInteger = apaOffVal + apaMulVal * (aplName % aplNELMVal);

                // Assign this token to this name
                AssignName_EM (&((LPYYSTYPE) lpMemStr)[(aplNELMStr - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_LIST:
        defstop
            break;
    } // End SWITCH
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbVal); lpMemVal = NULL;
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbStr); lpMemStr = NULL;

    // Mark as not displayable
    lptkVal->tkFlags.NoDisplay = 1;

    DBGLEAVE;

    return bRet;
} // End AssignNameSpec_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: assign.c
//***************************************************************************
