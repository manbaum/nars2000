//***************************************************************************
//  NARS2000 -- Assignment Functions
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "datatype.h"
#include "resdebug.h"
#include "Unicode.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  AssignName_EM
//
//  Assign a value to a name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignName_EM"
#else
#define APPEND_NAME
#endif

BOOL AssignName_EM
    (LPYYSTYPE lpYYName,
     LPYYSTYPE lpYYExpr)

{
    STFLAGS stFlags = {0};
    HGLOBAL hGlbExpr;

    // Because when the tokens are first created we don't
    //   know the type of a name, it is arbitrarily typed
    //   as TKT_VARNAMED.  Now is the time to set the record
    //   straight.
    SetNameType (&lpYYName->tkToken);

    // It's a named variable or function
    Assert (lpYYName->tkToken.tkFlags.TknType EQ TKT_VARNAMED
         || lpYYName->tkToken.tkFlags.TknType EQ TKT_FCNNAMED
         || lpYYName->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
         || lpYYName->tkToken.tkFlags.TknType EQ TKT_OP1NAMED
         || lpYYName->tkToken.tkFlags.TknType EQ TKT_OP2NAMED);

    // If this is a system var, validate the assignment
    //   before we free the old value
    if (lpYYName->tkToken.tkData.lpSym->stFlags.SysVar)
    {
        // Validate the value
        if (!(*aSysVarValid[lpYYName->tkToken.tkData.lpSym->stFlags.SysVarValid]) (lpYYName, &lpYYExpr->tkToken))
            return FALSE;

        // If the result is immediate (a scalar), quit as we
        //   might have extracted a scalar from a singleton array.
        if (lpYYName->tkToken.tkData.lpSym->stFlags.Imm)
            return TRUE;
    } // End IF

    // Note that we have to wait until all errors have been
    //   resolved before calling FreeResultName.

    // Split cases based upon the expression token type
    switch (lpYYExpr->tkToken.tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYExpr->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If the expression is not immediate, ...
            if (!lpYYExpr->tkToken.tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable or function array
                Assert (IsGlbTypeVarDir (lpYYExpr->tkToken.tkData.lpSym->stData.stGlbData)
                     || IsGlbTypeFcnDir (lpYYExpr->tkToken.tkData.lpSym->stData.stGlbData));

                // ***FIXME*** -- Do we need the tests below??
                //                Try []WSID {is} []SA
                //                and []WSID {is} []WSID

////////////////// There are no reference counts on .SysName variables
////////////////if (lpYYName->tkToken.tkData.lpSym->stFlags.SysVar
//////////////// || lpYYExpr->tkToken.tkData.lpSym->stFlags.SysVar)
////////////////{
                    hGlbExpr = CopyArray_EM (ClrPtrTypeDirGlb (lpYYExpr->tkToken.tkData.lpSym->stData.stGlbData),
                                             FALSE,
                                             &lpYYExpr->tkToken);
                    if (!hGlbExpr)
                        return FALSE;

                    // Free the old value for this name
                    FreeResultName (lpYYName);

                    // Save the new global memory ptr
                    lpYYName->tkToken.tkData.lpSym->stData.stGlbData = MakeGlbTypeGlb (hGlbExpr);

                    break;
////////////////} // End IF
////////////////
////////////////// If the global memory ptrs are different, there's no
//////////////////   need to copy the expression's ptr and it would be
//////////////////   a mistake to increment the reference count
////////////////if (lpYYName->tkToken.tkData.lpSym->stData.stGlbData NE
////////////////    lpYYExpr->tkToken.tkData.lpSym->stData.stGlbData)
////////////////{
////////////////    // Copy the global memory ptr
////////////////    lpYYName->tkToken.tkData.lpSym->stData.stGlbData =
////////////////    lpYYExpr->tkToken.tkData.lpSym->stData.stGlbData;
////////////////
////////////////    // Increment the reference count in global memory
////////////////    DbgIncrRefCntDir (lpYYName->tkToken.tkData.lpSym->stData.stGlbData);
////////////////} // End IF
////////////////
////////////////break;
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYExpr->tkToken.tkData.lpVoid) EQ PTRTYPE_STCONST);

            // stData is immediate
            Assert (lpYYExpr->tkToken.tkData.lpSym->stFlags.Imm);

            // Free the old value for this name
            FreeResultName (lpYYName);

            // Because the source and the destination
            //   may well have very different characteristics
            //   (SysVar vs. UsrVar), we can't re-use the
            //   source LPSYMENTRY, so we copy the values
            //   into the target's LPSYMENTRY.

            // Copy the expression's flags
            stFlags = lpYYExpr->tkToken.tkData.lpSym->stFlags;

            // Include the expression's .Imm & .ImmType flags
            lpYYName->tkToken.tkData.lpSym->stFlags.Imm     = stFlags.Imm;
            lpYYName->tkToken.tkData.lpSym->stFlags.ImmType = stFlags.ImmType;

            // Copy the constant data
            lpYYName->tkToken.tkData.lpSym->stData = lpYYExpr->tkToken.tkData.lpSym->stData;

            break;

        case TKT_VARIMMED:
            // Free the old value for this name
            FreeResultName (lpYYName);

            // Because the source and the destination
            //   may well have very different characteristics
            //   (SysVar vs. UsrVar), we can't re-use the
            //   source LPSYMENTRY, so we copy the values
            //   into the target's LPSYMENTRY.

            // It's an immediate
            lpYYName->tkToken.tkData.lpSym->stFlags.Imm = 1;

            // Include the expression's .ImmType flags
            lpYYName->tkToken.tkData.lpSym->stFlags.ImmType =
                          lpYYExpr->tkToken.tkFlags.ImmType;

            // Copy the constant data
            lpYYName->tkToken.tkData.lpSym->stData.stLongest=
                          lpYYExpr->tkToken.tkData.tkLongest;
            break;

        case TKT_FCNIMMED:
            // Free the old value for this name
            FreeResultName (lpYYName);

            // It's an immediate primitive function
            lpYYName->tkToken.tkData.lpSym->stFlags.Imm     = 1;
            lpYYName->tkToken.tkData.lpSym->stFlags.ImmType = IMMTYPE_PRIMFCN;
            lpYYName->tkToken.tkData.lpSym->stFlags.UsrFn12 = 1;

            // Copy the constant data
            lpYYName->tkToken.tkData.lpSym->stData.stLongest=
                          lpYYExpr->tkToken.tkData.tkLongest;
            break;

        case TKT_OP1IMMED:
            // Free the old value for this name
            FreeResultName (lpYYName);

            // It's an immediate primitive function
            lpYYName->tkToken.tkData.lpSym->stFlags.Imm     = 1;
            lpYYName->tkToken.tkData.lpSym->stFlags.ImmType = IMMTYPE_PRIMOP1;
            lpYYName->tkToken.tkData.lpSym->stFlags.UsrOp1  = 1;

            // Copy the constant data
            lpYYName->tkToken.tkData.lpSym->stData.stLongest=
                          lpYYExpr->tkToken.tkData.tkLongest;
            break;

        case TKT_OP2IMMED:
            // Free the old value for this name
            FreeResultName (lpYYName);

            // It's an immediate primitive function
            lpYYName->tkToken.tkData.lpSym->stFlags.Imm     = 1;
            lpYYName->tkToken.tkData.lpSym->stFlags.ImmType = IMMTYPE_PRIMOP2;
            lpYYName->tkToken.tkData.lpSym->stFlags.UsrOp2  = 1;

            // Copy the constant data
            lpYYName->tkToken.tkData.lpSym->stData.stLongest=
                          lpYYExpr->tkToken.tkData.tkLongest;
            break;

        case TKT_VARARRAY:
            AssignArrayCommon (lpYYName, lpYYExpr, TKT_VARNAMED);

            break;

        case TKT_FCNARRAY:
            AssignArrayCommon (lpYYName, lpYYExpr, TKT_FCNNAMED);

            break;

        defstop
            break;
    } // End SWITCH

    // If this is a named function, mark it as such
    if (lpYYExpr->tkToken.tkFlags.TknType EQ TKT_FCNNAMED
     || lpYYExpr->tkToken.tkFlags.TknType EQ TKT_FCNARRAY)
    {
        // Split cases based upon the underlying FCNTYPE_xxx
        switch (GetFcnType (&lpYYExpr->tkToken))
        {
            case FCNTYPE_FCN:
            case FCNTYPE_AXISFCN:
                lpYYName->tkToken.tkData.lpSym->stFlags.UsrFn12 = 1;

                break;

            case FCNTYPE_OP1:
                lpYYName->tkToken.tkData.lpSym->stFlags.UsrOp1 = 1;

                break;

            case FCNTYPE_OP2:
                lpYYName->tkToken.tkData.lpSym->stFlags.UsrOp2 = 1;

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    // Mark as valued
    lpYYName->tkToken.tkData.lpSym->stFlags.Value = 1;

    // Ensure either .SysVar or .UsrVar is set
    if (lpYYName->tkToken.tkData.lpSym->stFlags.SysName
     && !lpYYName->tkToken.tkData.lpSym->stFlags.SysFn12)
        lpYYName->tkToken.tkData.lpSym->stFlags.SysVar = 1;
    else
    if (lpYYName->tkToken.tkData.lpSym->stFlags.UsrName
     && !lpYYName->tkToken.tkData.lpSym->stFlags.UsrFn12
     && !lpYYName->tkToken.tkData.lpSym->stFlags.UsrOp1
     && !lpYYName->tkToken.tkData.lpSym->stFlags.UsrOp2)
        lpYYName->tkToken.tkData.lpSym->stFlags.UsrVar = 1;

    // Mark as not displayable
    lpYYName->tkToken.tkFlags.NoDisplay = 1;

    Assert (HshTabFrisk ());

#ifdef DEBUG
    DisplayFcnStrand (&lpYYName->tkToken);
#endif
    return TRUE;
} // End AssignName_EM
#undef  APPEND_NAME


//***************************************************************************
//  SetNameType
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
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, ...
            if (!lpToken->tkData.lpSym->stFlags.Imm)
            {
                // If it has no value, exit
                if (!lpToken->tkData.lpSym->stFlags.Value)
                    return;

                // stData is an HGLOBAL function array
                Assert (IsGlbTypeFcnDir (lpToken->tkData.lpSym->stData.stGlbData));

                // Set the new token type
                lpToken->tkFlags.TknType =
                  TranslateFcnTypeToTknTypeNamed (GetFcnType (lpToken));
            } else
                lpToken->tkFlags.TknType =
                  TranslateImmTypeToTknType (lpToken->tkData.lpSym->stFlags.ImmType);
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
//  GetFcnType
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
            Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate, ...
            if (!lpToken->tkData.lpSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.lpSym->stData.stGlbData;

                break;      // Continue with common hGlbData code
            } // End IF

            // Split cases based upon the immediate token type
            switch (lpToken->tkData.lpSym->stFlags.ImmType)
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

    // Lock the global memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)

    cFcnType = lpHeader->FcnType;

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    return cFcnType;
} // End GetFcnType


//***************************************************************************
//  AssignArrayCommon
//
//  Assign either a variable or function array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignArrayCommon_EM"
#else
#define APPEND_NAME
#endif

void AssignArrayCommon
    (LPYYSTYPE   lpYYName,
     LPYYSTYPE   lpYYExpr,
     TOKEN_TYPES TknType)

{
    // Free the old value for this name
    FreeResultName (lpYYName);

    // Fill in the token type
    lpYYName->tkToken.tkFlags.TknType = TknType;

    // Copy the HGLOBAL
    lpYYName->tkToken.tkData.lpSym->stData.stGlbData =
      MakeGlbTypeGlb (CopyArray_EM (lpYYExpr->tkToken.tkData.tkGlbData, FALSE, &lpYYExpr->tkToken));

////// Mark the expression's global ptr as reused
////lpYYExpr->tkToken.tkData.tkGlbData = PTR_REUSED;

////// Increment the reference count in global memory
////DbgIncrRefCntDir (lpYYName->tkToken.tkData.lpSym->stData.stGlbData);
} // End AssignArrayCommon
#undef  APPEND_NAME


//***************************************************************************
//  End of File: assign.c
//***************************************************************************
