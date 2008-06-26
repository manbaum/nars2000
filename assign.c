//***************************************************************************
//  NARS2000 -- Assignment Functions
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
#include "dfnhdr.h"

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

UBOOL AssignName_EM
    (LPTOKEN lptkNam,               // Ptr to name token
     LPTOKEN lptkSrc)               // Ptr to source token

{
    STFLAGS stSrcFlags = {0},   // Copy of the source's STE flags
            stNamFlags;         // ...         name's   ...
    HGLOBAL hGlbSrc;            // Source's global memory handle
    UBOOL   bFcnOpr,            // TRUE iff source is a function/operator
            bRet = TRUE;        // TRUE iff result is valid

    DBGENTER;

    // It's a named variable or function
    Assert (lptkNam->tkFlags.TknType EQ TKT_VARNAMED
         || lptkNam->tkFlags.TknType EQ TKT_FCNNAMED
         || lptkNam->tkFlags.TknType EQ TKT_FCNIMMED
         || lptkNam->tkFlags.TknType EQ TKT_OP1NAMED
         || lptkNam->tkFlags.TknType EQ TKT_OP2NAMED
         || lptkNam->tkFlags.TknType EQ TKT_OP3NAMED);

    // If the target is a system var, validate the assignment
    //   before we free the old value
    if (IsNameTypeVar (lptkNam->tkData.tkSym->stFlags.stNameType)
     && lptkNam->tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS)
    {
        // If the target is a user-defined function/operator system label, signal a SYNTAX ERROR
        if (lptkNam->tkData.tkSym->stFlags.DfnSysLabel)
            goto SYNTAX_EXIT;

        // Validate the value
        return (*aSysVarValidSet[lptkNam->tkData.tkSym->stFlags.SysVarValid]) (lptkNam, lptkSrc);
    } // End IF

    // Note that we have to wait until all errors have been
    //   resolved before calling FreeResultName.

    // Split cases based upon the source token type
    switch (lptkSrc->tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkSrc->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Ensure there's a value (could have come from a{is}{execute}'')
            if (IsSymNoValue (lptkSrc->tkData.tkSym))
                goto VALUE_EXIT;

            // If the target is a user-defined function/operator label, signal a SYNTAX ERROR
            if (lptkNam->tkData.tkSym->stFlags.DfnLabel)
                goto SYNTAX_EXIT;

            // If the source is immediate, ...
            if (lptkSrc->tkData.tkSym->stFlags.Imm)
            {
                // Handle the immediate case

                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkSrc->tkData.tkVoid) EQ PTRTYPE_STCONST);

                // stData is immediate
                Assert (lptkSrc->tkData.tkSym->stFlags.Imm);

                // Free the old value for this name
                FreeResultName (lptkNam);

                // Because the source and the destination
                //   may well have very different characteristics
                //   (SysVar vs. UsrVar), we can't re-use the
                //   source LPSYMENTRY, so we copy the values
                //   into the target's LPSYMENTRY.

                // Copy the source's flags
                stSrcFlags = lptkSrc->tkData.tkSym->stFlags;

                // Include the source's .Imm & .ImmType flags
                lptkNam->tkData.tkSym->stFlags.Imm      =
                                    stSrcFlags.Imm;
                lptkNam->tkData.tkSym->stFlags.ImmType  =
                                    stSrcFlags.ImmType;
                // Copy the constant data
                lptkNam->tkData.tkSym->stData.stLongest =
                lptkSrc->tkData.tkSym->stData.stLongest;
            } else
            {
                // Handle the global case

                // stData is an internal function, a valid HGLOBAL variable or function array,
                //   or user-defined function/operator
                Assert (lptkSrc->tkData.tkSym->stFlags.FcnDir
                     || IsGlbTypeVarDir (lptkSrc->tkData.tkSym->stData.stGlbData)
                     || IsGlbTypeFcnDir (lptkSrc->tkData.tkSym->stData.stGlbData)
                     || IsGlbTypeDfnDir (lptkSrc->tkData.tkSym->stData.stGlbData));

                // Get the source global memory handle
                hGlbSrc = lptkSrc->tkData.tkSym->stData.stGlbData;

                // Free the old value for this name
                FreeResultName (lptkNam);

                // Clear the immediate flag
                lptkNam->tkData.tkSym->stFlags.Imm = FALSE;
                lptkNam->tkData.tkSym->stFlags.ImmType = 0;

                // Copy the "Accepts Axis Operator" flag
                lptkNam->tkData.tkSym->stFlags.DfnAxis =
                lptkSrc->tkData.tkSym->stFlags.DfnAxis;

                // Check for internal functions
                if (lptkSrc->tkData.tkSym->stFlags.FcnDir)
                {
                    // Copy the SYMENTRY's data
                    lptkNam->tkData.tkSym->stData =
                    lptkSrc->tkData.tkSym->stData;

                    // Copy the direct function flag
                    lptkNam->tkData.tkSym->stFlags.FcnDir =
                    lptkSrc->tkData.tkSym->stFlags.FcnDir;
                } else
                {
                    // Copy the source global memory handle
                    //   and save it as the new global memory ptr
                    lptkNam->tkData.tkSym->stData.stGlbData = CopySymGlbDir (hGlbSrc);

                    // Transfer user-defined function/operator flag
                    lptkNam->tkData.tkSym->stFlags.UsrDfn =
                    lptkSrc->tkData.tkSym->stFlags.UsrDfn;
                } // End IF/ELSE
            } // End IF/ELSE

            break;

        case TKT_VARIMMED:
            // Free the old value for this name
            FreeResultName (lptkNam);

            // Because the source and the destination
            //   may well have very different characteristics
            //   (SysVar vs. UsrVar), we can't re-use the
            //   source LPSYMENTRY, so we copy the values
            //   into the target's LPSYMENTRY.

            // It's an immediate
            lptkNam->tkData.tkSym->stFlags.Imm = TRUE;

            // Include the source's .ImmType flags
            lptkNam->tkData.tkSym->stFlags.ImmType =
                           lptkSrc->tkFlags.ImmType;

            // Copy the constant data
            lptkNam->tkData.tkSym->stData.stLongest=
                           lptkSrc->tkData.tkLongest;
            break;

        case TKT_FCNIMMED:
            // Free the old value for this name
            FreeResultName (lptkNam);

            // It's an immediate primitive function
            lptkNam->tkData.tkSym->stFlags.Imm     = TRUE;
            lptkNam->tkData.tkSym->stFlags.ImmType = GetImmTypeFcn (lptkSrc->tkData.tkChar);
            lptkNam->tkData.tkSym->stFlags.stNameType = NAMETYPE_FN12;

            // Copy the constant data
            lptkNam->tkData.tkSym->stData.stLongest=
                           lptkSrc->tkData.tkLongest;
            break;

        case TKT_OP1IMMED:
            // Free the old value for this name
            FreeResultName (lptkNam);

            // It's an immediate primitive operator
            lptkNam->tkData.tkSym->stFlags.Imm     = TRUE;
            lptkNam->tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMOP1;
            lptkNam->tkData.tkSym->stFlags.stNameType = NAMETYPE_OP1;

            // Copy the constant data
            lptkNam->tkData.tkSym->stData.stLongest=
                           lptkSrc->tkData.tkLongest;
            break;

        case TKT_OP2IMMED:
            // Free the old value for this name
            FreeResultName (lptkNam);

            // It's an immediate primitive operator
            lptkNam->tkData.tkSym->stFlags.Imm     = TRUE;
            lptkNam->tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMOP2;
            lptkNam->tkData.tkSym->stFlags.stNameType = NAMETYPE_OP2;

            // Copy the constant data
            lptkNam->tkData.tkSym->stData.stLongest=
                           lptkSrc->tkData.tkLongest;
            break;

        case TKT_OP3IMMED:
            // Free the old value for this name
            FreeResultName (lptkNam);

            // It's an immediate primitive operator
            lptkNam->tkData.tkSym->stFlags.Imm     = TRUE;
            lptkNam->tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMOP3;
            lptkNam->tkData.tkSym->stFlags.stNameType = NAMETYPE_OP3;

            // Copy the constant data
            lptkNam->tkData.tkSym->stData.stLongest=
                           lptkSrc->tkData.tkLongest;
            break;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable
            Assert (IsGlbTypeVarDir (lptkSrc->tkData.tkVoid));

            // Call function common to TKT_VARARRAY and TKT_FCNARRAY
            AssignArrayCommon (lptkNam, lptkSrc, TKT_VARNAMED);

            break;

        case TKT_FCNARRAY:
            // tkData is a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir (lptkSrc->tkData.tkVoid));

            // Call function common to TKT_VARARRAY and TKT_FCNARRAY
            AssignArrayCommon (lptkNam, lptkSrc, TKT_FCNNAMED);

            break;

        defstop
            break;
    } // End SWITCH

    // If the source is not an internal function
    //   and is a named function or operator,
    //   mark the source with its type
    if ((lptkSrc->tkFlags.TknType EQ TKT_FCNNAMED
      || lptkSrc->tkFlags.TknType EQ TKT_OP1NAMED
      || lptkSrc->tkFlags.TknType EQ TKT_OP2NAMED
      || lptkSrc->tkFlags.TknType EQ TKT_OP3NAMED)
     && !lptkSrc->tkData.tkSym->stFlags.FcnDir)     // Valid as the TknType is TKT_xxxNAMED
        lptkSrc->tkData.tkSym->stFlags.stNameType = GetNameType (lptkSrc);

    // If the source is a function or operator
    //   mark the name as such
    bFcnOpr = (lptkSrc->tkFlags.TknType EQ TKT_FCNNAMED
            || lptkSrc->tkFlags.TknType EQ TKT_FCNIMMED
            || lptkSrc->tkFlags.TknType EQ TKT_FCNARRAY
            || lptkSrc->tkFlags.TknType EQ TKT_OP1NAMED
            || lptkSrc->tkFlags.TknType EQ TKT_OP1IMMED
            || lptkSrc->tkFlags.TknType EQ TKT_OP2NAMED
            || lptkSrc->tkFlags.TknType EQ TKT_OP2IMMED
            || lptkSrc->tkFlags.TknType EQ TKT_OP3NAMED
            || lptkSrc->tkFlags.TknType EQ TKT_OP3IMMED);
    if (bFcnOpr)
    {
        // Set the object type
        lptkNam->tkData.tkSym->stFlags.stNameType = GetNameType (lptkSrc);

        // Split cases based upon the underlying NAMETYPE_xxx
        switch (lptkNam->tkData.tkSym->stFlags.stNameType)
        {
            case NAMETYPE_FN0:
                lptkNam->tkFlags.TknType = TKT_FCNNAMED;

                break;

            case NAMETYPE_FN12:
                lptkNam->tkFlags.TknType = TKT_FCNNAMED;

                break;

            case NAMETYPE_OP1:
                lptkNam->tkFlags.TknType = TKT_OP1NAMED;

                break;

            case NAMETYPE_OP2:
                lptkNam->tkFlags.TknType = TKT_OP2NAMED;

                break;

            case NAMETYPE_OP3:
                lptkNam->tkFlags.TknType = TKT_OP3NAMED;

                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    // Mark as valued
    lptkNam->tkData.tkSym->stFlags.Value = TRUE;

    // If it's a var, ensure NAMETYPE_VAR is set for either .stNameType
    stNamFlags = lptkNam->tkData.tkSym->stFlags;
    if (!IsNameTypeFnOp (stNamFlags.stNameType))
        lptkNam->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;

    // Mark as not displayable
    lptkNam->tkFlags.NoDisplay = TRUE;

    Assert (HshTabFrisk (FALSE));

#ifdef DEBUG
    if (bFcnOpr)
        DisplayFcnStrand (lptkSrc);
#endif

    goto NORMAL_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                               lptkSrc);
    goto ERROR_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkNam);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    DBGLEAVE;

    return bRet;
} // End AssignName_EM
#undef  APPEND_NAME


//***************************************************************************
//  $GetNameType
//
//  Return the NAMETYPE_xxx of a function token
//***************************************************************************

NAME_TYPES GetNameType
    (LPTOKEN lptkFunc)              // Ptr to function token

{
    HGLOBAL    hGlbData;            // Function array global memory handle
    LPVOID     lpMem;               // Ptr to function array global memory
    NAME_TYPES fnNameType;          // Function name type (see NAME_TYPES)

    // Split cases based upon the token type
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkFunc->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate, ...
            if (!lptkFunc->tkData.tkSym->stFlags.Imm)
            {
                // Check for internal functions
                if (lptkFunc->tkData.tkSym->stFlags.FcnDir)
                {
                    // This means that stGlbData is a direct function address

                    // Check for niladic functions
                    if (lptkFunc->tkData.tkSym->stFlags.stNameType EQ NAMETYPE_FN0)
                        return NAMETYPE_FN0;

                    // Check for monadic/dyadic/ambivalent functions
                    if (lptkFunc->tkData.tkSym->stFlags.stNameType EQ NAMETYPE_FN12)
                        return NAMETYPE_FN12;

                    DbgStop ();         // We should never get here
                                        //   unless we define some
                                        //   internal operators
                } // End IF

                // Get the HGLOBAL
                hGlbData = lptkFunc->tkData.tkSym->stData.stGlbData;

                break;      // Continue with common hGlbData code
            } // End IF

            // Split cases based upon the immediate token type
            switch (lptkFunc->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_PRIMFCN:
                    return NAMETYPE_FN12;

                case IMMTYPE_PRIMOP1:
                    return NAMETYPE_OP1;

                case IMMTYPE_PRIMOP2:
                    return NAMETYPE_OP2;

                case IMMTYPE_PRIMOP3:
                    return NAMETYPE_OP3;

                defstop
                    break;
            } // End SWITCH

            return -1;

        case TKT_FCNIMMED:
            return NAMETYPE_FN12;

        case TKT_OP1NAMED:
        case TKT_OP1IMMED:
            return NAMETYPE_OP1;

        case TKT_OP2NAMED:
        case TKT_OP2IMMED:
            return NAMETYPE_OP2;

        case TKT_OP3NAMED:
        case TKT_OP3IMMED:
            return NAMETYPE_OP3;

        case TKT_FCNARRAY:
            hGlbData = lptkFunc->tkData.tkGlbData;

            break;      // Continue with common hGlbData code

        defstop
            return -1;
    } // End SWITCH

    // stData/tkData is a valid HGLOBAL function array
    //   or user-defined function/operator
    Assert (IsGlbTypeFcnDir (hGlbData)
         || IsGlbTypeDfnDir (hGlbData));

    // Clear the ptr type bits
    hGlbData = ClrPtrTypeDirAsGlb (hGlbData);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Split cases based upon the signature
    switch (((LPHEADER_SIGNATURE) lpMem)->nature)
    {
#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
        case FCNARRAY_HEADER_SIGNATURE:
            fnNameType = lpHeader->fnNameType;

            break;
#undef  lpHeader

#define lpHeader    ((LPDFN_HEADER) lpMem)
        case DFN_HEADER_SIGNATURE:
            fnNameType = TranslateDfnToNameType (lpHeader->DfnType, lpHeader->FcnValence);

            break;
#undef  lpHeader

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    return fnNameType;
} // End GetNameType


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
    (LPTOKEN     lptkName,          // Ptr to name token
     LPTOKEN     lptkSrc,           // Ptr to source token
     TOKEN_TYPES TknType)           // Incoming token type for name (see TOKEN_TYPES)

{
    // Free the old value for this name
    FreeResultName (lptkName);

    // Fill in the token type
    lptkName->tkFlags.TknType = TknType;

    // Copy the HGLOBAL
    lptkName->tkData.tkSym->stData.stGlbData = CopySymGlbDir (lptkSrc->tkData.tkGlbData);
} // End AssignArrayCommon
#undef  APPEND_NAME


//***************************************************************************
//  $AssignSelectSpec_EM
//
//  Assign values to selective specification (currently a name strand)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AssignSelectSpec_EM"
#else
#define APPEND_NAME
#endif

UBOOL AssignSelectSpec_EM
    (LPTOKEN       lptkStr,         // Ptr to named strand token
     LPTOKEN       lptkVal)         // Ptr to value token

{
    UBOOL      bRet = TRUE;     // TRUE iff result is valid
    HGLOBAL    hGlbStr,         // Name strand global memory handle
               hGlbVal,         // Value       ...
               hGlbSub;         // Subarray    ...
    LPVOID     lpMemNam,        // Ptr to name strand global memory
               lpMemVal;        // Ptr to value
    APLNELM    aplNELMNam,      // Name strand NELM
               aplNELMVal,      // Value ...
               aplName;         // Loop counter
    APLRANK    aplRankVal;
    APLSTYPE   aplTypeVal;
    TOKEN      tkToken = {0};
    UINT       uBitMaskVal;
    LPSYMENTRY lpSymVal;
    APLINT     apaOffVal,
               apaMulVal;

    DBGENTER;

    Assert (lptkStr->tkFlags.TknType EQ TKT_STRNAMED);

    // Get the global handle
    hGlbStr = ClrPtrTypeDirAsGlb (lptkStr->tkData.tkGlbData);

    // Lock the memory to get a ptr to it
    lpMemNam = MyGlobalLock (hGlbStr);

#define lpHeader    ((LPVARNAMED_HEADER) lpMemNam)
    // Get the # names in the strand
    aplNELMNam = lpHeader->NELM;
#undef  lpHeader

    // Skip over the name strand header to the data
    lpMemNam = VarNamedBaseToData (lpMemNam);

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
            for (aplName = 0; aplName < aplNELMNam; aplName++)
                AssignName_EM (&((LPPL_YYSTYPE) lpMemNam)[aplName].tkToken, lptkVal);
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
    hGlbVal = ClrPtrTypeDirAsGlb (hGlbVal);

    // Lock the memory to get a ptr to it
    lpMemVal = MyGlobalLock (hGlbVal);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemVal)
    // Get the Type, NELM, and Rank
    aplTypeVal = lpHeader->ArrType;
    aplRankVal = lpHeader->Rank;
    aplNELMVal = lpHeader->NELM;
#undef  lpHeader

    // Check for RANK ERROR
    if (IsMultiRank (aplRankVal))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (!IsSingleton (aplNELMVal)
     && aplNELMVal NE aplNELMNam)
        goto LENGTH_EXIT;

    // Skip over the header and dimension to the data
    lpMemVal = VarArrayBaseToData (lpMemVal, aplRankVal);

    // If the value is an APA, ...
    if (IsSimpleAPA (aplTypeVal))
    {
#define lpAPA       ((LPAPLAPA) lpMemVal)
        // Get the APA parameters
        apaOffVal = lpAPA->Off;
        apaMulVal = lpAPA->Mul;
#undef  lpAPA
    } // End IF

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
            for (aplName = 0; aplName < aplNELMNam; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkBoolean = (uBitMaskVal & *(LPAPLBOOL) lpMemVal) ? 1 : 0;

                // Assign this token to this name
                AssignName_EM (&((LPPL_YYSTYPE) lpMemNam)[(aplNELMNam - 1) - aplName].tkToken, &tkToken);

                // If there's more than one value, ...
                if (!IsSingleton (aplNELMVal))
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
            for (aplName = 0; aplName < aplNELMNam; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkInteger = ((LPAPLINT) lpMemVal)[aplName % aplNELMVal];

                // Assign this token to this name
                AssignName_EM (&((LPPL_YYSTYPE) lpMemNam)[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_FLOAT:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_FLOAT;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMNam; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkFloat = ((LPAPLFLOAT) lpMemVal)[aplName % aplNELMVal];

                // Assign this token to this name
                AssignName_EM (&((LPPL_YYSTYPE) lpMemNam)[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_CHAR;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMNam; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkChar = ((LPAPLCHAR) lpMemVal)[aplName % aplNELMVal];

                // Assign this token to this name
                AssignName_EM (&((LPPL_YYSTYPE) lpMemNam)[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_HETERO:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMNam; aplName++)
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
                AssignName_EM (&((LPPL_YYSTYPE) lpMemNam)[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_NESTED:
            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMNam; aplName++)
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
                AssignName_EM (&((LPPL_YYSTYPE) lpMemNam)[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_APA:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_INT;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMNam; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkInteger = apaOffVal + apaMulVal * (aplName % aplNELMVal);

                // Assign this token to this name
                AssignName_EM (&((LPPL_YYSTYPE) lpMemNam)[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_LIST:
        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkVal);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkVal);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;
NORMAL_EXIT:
    if (hGlbVal && lpMemVal)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbVal); lpMemVal = NULL;
    } // End IF

    if (hGlbStr && lpMemNam)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbStr); lpMemNam = NULL;
    } // End IF

    // Mark as not displayable
    lptkVal->tkFlags.NoDisplay = 1;

    DBGLEAVE;

    return bRet;
} // End AssignSelectSpec_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ModifyAssignNameVals_EM
//
//  Assign modified values to a name strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ModifyAssignNameVals_EM"
#else
#define APPEND_NAME
#endif

UBOOL ModifyAssignNameVals_EM
    (LPTOKEN       lptkStrN,        // Ptr to name strand token
     LPPL_YYSTYPE  lpYYFcnStr,      // Ptr to function strand
     LPTOKEN       lptkVal)         // Ptr to value token

{
    HGLOBAL      hGlbName;          // Name strand global memory handle
    LPPL_YYSTYPE lpMemName;         // Ptr to name strand global memory
    APLNELM      aplNELMNam;        // Name strand NELM
    APLUINT      uName;             // Loop counter
    UBOOL        bRet = FALSE;      // TRUE iff result is valid

    // Get the name strand global memory handle
    hGlbName = lptkStrN->tkData.tkGlbData;

    // tkData is a valid HGLOBAL name strand
    Assert (IsGlbTypeNamDir (hGlbName));

    // Clear the ptr type bits
    hGlbName = ClrPtrTypeDirAsGlb (hGlbName);

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (hGlbName);

#define lpHeader        ((LPVARNAMED_HEADER) lpMemName)
    aplNELMNam = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header to the data
    lpMemName = VarNamedBaseToData (lpMemName);

    // Loop through the names
    for (uName = 0; uName < aplNELMNam; uName++)
    {
        LPPL_YYSTYPE lpYYRes;       // Ptr to the result

        lpYYRes =
          ExecFunc_EM_YY (&lpMemName[uName].tkToken, lpYYFcnStr, lptkVal);

        if (lpYYRes)
        {
            bRet = AssignName_EM (&lpMemName[uName].tkToken, &lpYYRes->tkToken);
            YYFree (lpYYRes); lpYYRes = NULL;

            if (!bRet)
                goto ERROR_EXIT;
        } else
            goto ERROR_EXIT;
    } // End FOR

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbName); lpMemName = NULL;

    return bRet;
} // End ModifyAssignNameVals_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: assign.c
//***************************************************************************
