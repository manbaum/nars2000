//***************************************************************************
//  NARS2000 -- Assignment Functions
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


//***************************************************************************
//  $AssignName_EM
//
//  Assign a token value to a token name
//***************************************************************************

UBOOL AssignName_EM
    (LPTOKEN lptkNam,               // Ptr to name token
     LPTOKEN lptkSrc)               // Ptr to source token

{
    STFLAGS      stSrcFlags = {0},  // Copy of the source's STE flags
                 stNamFlags;        // ...         name's   ...
    HGLOBAL      hGlbSrc;           // Source's global memory handle
    UBOOL        bFcnOpr,           // TRUE iff source is a function/operator
                 bFcnOprVar,        // TRUE iff source is a function/operator/variable
                 bRet = TRUE;       // TRUE iff result is valid
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to UDFO header

    // It's a named variable, function, or operator
    Assert (lptkNam->tkFlags.TknType EQ TKT_VARNAMED
         || lptkNam->tkFlags.TknType EQ TKT_FCNNAMED
         || lptkNam->tkFlags.TknType EQ TKT_FCNIMMED

         || lptkNam->tkFlags.TknType EQ TKT_FCNAFO
         || lptkNam->tkFlags.TknType EQ TKT_OP1AFO
         || lptkNam->tkFlags.TknType EQ TKT_OP2AFO

         || lptkNam->tkFlags.TknType EQ TKT_OP1NAMED
         || lptkNam->tkFlags.TknType EQ TKT_OP2NAMED
         || lptkNam->tkFlags.TknType EQ TKT_OP3NAMED);

    // If the target is a system var, validate the assignment
    //   before we free the old value
    if (IsNameTypeVar (lptkNam->tkData.tkSym->stFlags.stNameType)
     && lptkNam->tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS)
    {
        // If the target is a user-defined function/operator system label,
        //   or the source is a function/operator, ...
        if (lptkNam->tkData.tkSym->stFlags.DfnSysLabel
         || IsTknFcnOpr (lptkSrc))
            // Signal a SYNTAX ERROR
            goto SYNTAX_EXIT;

        // Validate the value
        bRet = (*aSysVarValidSet[lptkNam->tkData.tkSym->stFlags.SysVarValid]) (lptkNam, lptkSrc);

        // If it validated, ...
        if (bRet)
            // Execute the post-validation function
            (*aSysVarValidPost[lptkNam->tkData.tkSym->stFlags.SysVarValid]) (lptkNam);
        return bRet;
    } // End IF

    // Note that we have to wait until all errors have been
    //   resolved before calling FreeResultName.

    // If the target is a NoValue system name (except for []Z), ...
    if (IsTokenNoValue (lptkNam)
     && IsTknSysName   (lptkNam, TRUE))
        // Signal a VALUE ERROR
        goto NAME_VALUE_EXIT;

    // If the target is a user-defined function/operator label,
    //   or it's an internal function, ...
    if (lptkNam->tkData.tkSym->stFlags.DfnLabel
     || (lptkNam->tkData.tkSym->stFlags.FcnDir
      && lptkNam->tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS
      && IsTknSysName (lptkNam, TRUE)))
        // Signal a SYNTAX ERROR
        goto SYNTAX_EXIT;

    // If the name is suspended or pendent, it's not eraseable
    if (IzitSusPendent (lptkNam->tkData.tkSym))
        // Signal a SYNTAX ERROR
        goto SYNTAX_EXIT;

    // Split cases based upon the source token type
    switch (lptkSrc->tkFlags.TknType)
    {
        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
        case TKT_FCNDFN:
        case TKT_OP1DFN:
        case TKT_OP2DFN:
            // Get the source global memory handle
            hGlbSrc = GetGlbHandle (lptkSrc);

            // tkData is a user-defined function/operator
            Assert (IsGlbTypeDfnDir_PTB (hGlbSrc));

            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLockDfn (hGlbSrc);

            // Free the old value for this name
            FreeResultName (lptkNam);

            // Clear the name's STE flags
            ZeroMemory (&lptkNam->tkData.tkSym->stFlags, sizeof (lptkNam->tkData.tkSym->stFlags));

            // Is the name being assigned into an MFO?
            lpMemDfnHdr->bMFO = IsMFOName (lptkNam->tkData.tkSym->stHshEntry->lpwCharName);

            // Set STE flags
            lptkNam->tkData.tkSym->stFlags.Imm         = FALSE;
            lptkNam->tkData.tkSym->stFlags.ImmType     = IMMTYPE_ERROR;
            lptkNam->tkData.tkSym->stFlags.Inuse       =
            lptkNam->tkData.tkSym->stFlags.Value       = TRUE;
            lptkNam->tkData.tkSym->stFlags.ObjName     = lpMemDfnHdr->bMFO ? OBJNAME_MFO : OBJNAME_USR;
            lptkNam->tkData.tkSym->stFlags.stNameType  = GetNameType (lptkSrc);
////////////lptkNam->tkData.tkSym->stFlags.SysVarValid = 0          // Already zero from ZeroMemory
            lptkNam->tkData.tkSym->stFlags.UsrDfn      = TRUE;
////////////lptkNam->tkData.tkSym->stFlags.DfnLabel    = FALSE;     // Already zero from ZeroMemory
////////////lptkNam->tkData.tkSym->stFlags.DfnSysLabel = FALSE;     // Already zero from ZeroMemory
            lptkNam->tkData.tkSym->stFlags.DfnAxis     = lpMemDfnHdr->DfnAxis;
////////////lptkNam->tkData.tkSym->stFlags.FcnDir      = FALSE;     // Already zero from ZeroMemory

            // If the name being assigned to is not an MFO, ...
            if (!lpMemDfnHdr->bMFO)
                // Copy the UDFO (e.g., "f{is}g" where "g" may be any
                //   UDFO (not an AFO) other than a niladic function
                //   which instead is executed by the parser before
                //   it gets to this point.
                hGlbSrc = CopyUDFO (hGlbSrc, lptkNam->tkData.tkSym);

            // Save the new source global memory handle as the new global memory handle
            lptkNam->tkData.tkSym->stData.stGlbData    = hGlbSrc;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbSrc); lpMemDfnHdr = NULL;

            break;

        case TKT_VARNAMED:
        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkSrc->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Ensure there's a value (could have come from a{is}{execute}'')
            if (IsSymNoValue (lptkSrc->tkData.tkSym))
                goto ARG_VALUE_EXIT;

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
                     || IsGlbTypeVarDir_PTB (lptkSrc->tkData.tkSym->stData.stGlbData)
                     || IsGlbTypeFcnDir_PTB (lptkSrc->tkData.tkSym->stData.stGlbData)
                     || IsGlbTypeDfnDir_PTB (lptkSrc->tkData.tkSym->stData.stGlbData));

                // Get the source global memory handle
                hGlbSrc = lptkSrc->tkData.tkSym->stData.stGlbData;

                // Check for internal functions
                if (lptkSrc->tkData.tkSym->stFlags.FcnDir)
                {
                    // Free the old value for this name
                    FreeResultName (lptkNam);

                    // Copy the SYMENTRY's data
                    lptkNam->tkData.tkSym->stData =
                    lptkSrc->tkData.tkSym->stData;

                    // Copy the SYMENTRY's flags
                    lptkNam->tkData.tkSym->stFlags =
                    lptkSrc->tkData.tkSym->stFlags;
                } else
                {
                    HGLOBAL hGlbNam;            // Target's global memory handle
                    STFLAGS stFlags;            // Target's flags after <CopyUDFO>

                    // Copy the source global memory handle
                    //   and save it as the new global memory ptr
////////////////////lptkNam->tkData.tkSym->stData.stGlbData = CopySymGlbDir_PTB (hGlbSrc);

                    // Note that we can't free the target name before this call as we
                    //   need its characteristics in <CopyUDFO> as <lpSymName>
                    hGlbNam = CopyUDFO (hGlbSrc, lptkNam->tkData.tkSym);

                    // Make a copy of the target as we're about to free it
                    hGlbNam = CopySymGlbDir_PTB (hGlbNam);

                    // Save the target's flags to restore after <FreeResultName>
                    //   which clears the flags
                    stFlags = lptkNam->tkData.tkSym->stFlags;

                    // Free the old value for this name
                    FreeResultName (lptkNam);

                    // Restore the target's flags as set by <CopyUDFO>
                    lptkNam->tkData.tkSym->stFlags = stFlags;

                    // Save back into the name
                    lptkNam->tkData.tkSym->stData.stGlbData = hGlbNam;;

                    // Transfer user-defined function/operator flag
                    lptkNam->tkData.tkSym->stFlags.UsrDfn =
                    lptkSrc->tkData.tkSym->stFlags.UsrDfn;
                } // End IF/ELSE

                // Clear the immediate flag
                lptkNam->tkData.tkSym->stFlags.Imm     = FALSE;
                lptkNam->tkData.tkSym->stFlags.ImmType = IMMTYPE_ERROR;

                // Copy the "Accepts Axis Operator" flag
                lptkNam->tkData.tkSym->stFlags.DfnAxis =
                lptkSrc->tkData.tkSym->stFlags.DfnAxis;
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

            // Include the source's .ImmType flag
            lptkNam->tkData.tkSym->stFlags.ImmType =
                          lptkSrc->tkFlags.ImmType;

            // Copy the constant data
            lptkNam->tkData.tkSym->stData.stLongest   = *GetPtrTknLongest (lptkSrc);

            break;

        case TKT_FCNIMMED:
            // Free the old value for this name
            FreeResultName (lptkNam);

            // It's an immediate primitive function
            lptkNam->tkData.tkSym->stFlags.Imm        = TRUE;
            lptkNam->tkData.tkSym->stFlags.ImmType    = GetImmTypeFcn (lptkSrc->tkData.tkChar);
            lptkNam->tkData.tkSym->stFlags.stNameType = NAMETYPE_FN12;

            // Copy the constant data
            lptkNam->tkData.tkSym->stData.stLongest   = *GetPtrTknLongest (lptkSrc);

            break;

        case TKT_OP1IMMED:
        case TKT_OPJOTDOT:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
            // Free the old value for this name
            FreeResultName (lptkNam);

            // It's an immediate primitive operator
            lptkNam->tkData.tkSym->stFlags.Imm        = TRUE;
            lptkNam->tkData.tkSym->stFlags.stNameType = TranslateTknTypeToNameType (lptkSrc->tkFlags.TknType);
            lptkNam->tkData.tkSym->stFlags.ImmType    = TranslateNameTypeToImmType (lptkNam->tkData.tkSym->stFlags.stNameType);

            // Copy the constant data
            lptkNam->tkData.tkSym->stData.stLongest   = *GetPtrTknLongest (lptkSrc);

            break;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable
            Assert (IsGlbTypeVarDir_PTB (lptkSrc->tkData.tkVoid));

            // Call function common to TKT_VARARRAY and TKT_FCNARRAY
            AssignArrayCommon (lptkNam, lptkSrc, TKT_VARNAMED);

            break;

        case TKT_FCNARRAY:
        {
            TOKEN_TYPES tknType;

            // tkData is a valid HGLOBAL function array
            Assert (IsGlbTypeFcnDir_PTB (lptkSrc->tkData.tkVoid));

            // Distinguish between Fcns, OP1s, OP2s, and OP3s
            tknType = TranslateSOTypeToNamedTokenType (lptkSrc->tkSynObj);

            // Call function common to TKT_VARARRAY and TKT_FCNARRAY
            AssignArrayCommon (lptkNam, lptkSrc, tknType);

            break;
        } // End TKT_FCNARRAY

        defstop
            break;
    } // End SWITCH

    // If the source is not an internal function
    //   and is a           function, operator, or hyperator, ...
    bFcnOpr    = ((!IsTknNamed   (lptkSrc) || !lptkSrc->tkData.tkSym->stFlags.FcnDir)
               && IsTknFcnOpr    (lptkSrc));

    // If the source is not an internal function
    //   and is a variable, function, operator, or hyperator, ...
    bFcnOprVar = ((!IsTknNamed   (lptkSrc) || !lptkSrc->tkData.tkSym->stFlags.FcnDir)
               && IsTknFcnOprVar (lptkSrc));

    // If the source is not an internal function
    //   and is a variable, function, or operator, ...
    if (bFcnOprVar)
    {
        // Set the name type
        lptkNam->tkData.tkSym->stFlags.stNameType = GetNameType (lptkSrc);

        // Translate the NAMETYPE_xxx to the corresponding named token type
        lptkNam->tkFlags.TknType = TranslateNameTypeToTknTypeNamed (lptkNam->tkData.tkSym->stFlags.stNameType);
    } // End IF

    // Mark as Inuse and valued
    lptkNam->tkData.tkSym->stFlags.Inuse =
    lptkNam->tkData.tkSym->stFlags.Value = TRUE;

    // If it's a var, ensure NAMETYPE_VAR is set for either .stNameType
    stNamFlags = lptkNam->tkData.tkSym->stFlags;
    if (!IsNameTypeFnOp (stNamFlags.stNameType))
        lptkNam->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;

    // Mark as not displayable
    lptkSrc->tkFlags.NoDisplay = TRUE;

#ifdef DEBUG
{
    Assert (HshTabFrisk (GetMemPTD ()->lphtsPTD));

    if (bFcnOpr)
        DisplayFcnStrand (lptkSrc, TRUE);
}
#endif

    goto NORMAL_EXIT;

NAME_VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                               lptkNam);
    goto ERROR_EXIT;

ARG_VALUE_EXIT:
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
    return bRet;
} // End AssignName_EM


//***************************************************************************
//  $GetNameType
//
//  Return the NAMETYPE_xxx of a token
//***************************************************************************

NAME_TYPES GetNameType
    (LPTOKEN lptkFunc)              // Ptr to function token

{
    HGLOBAL      hGlbData;          // Function array global memory handle
    LPVOID       lpMem;             // Ptr to function array global memory
    NAME_TYPES   fnNameType;        // Function name type (see NAME_TYPES)

    // Split cases based upon the token type
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            return NAMETYPE_VAR;

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

        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
        case TKT_FCNDFN:
        case TKT_OP1DFN:
        case TKT_OP2DFN:
            // Set the UDFO/AFO properties
            return plSetDfn (lptkFunc, GetGlbDataToken (lptkFunc));

        case TKT_FCNIMMED:
        case TKT_OPJOTDOT:
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

        case TKT_FILLJOT:
            return NAMETYPE_UNK;

        case TKT_DEL:       // Del      -- always a function
        case TKT_DELDEL:    // Del Del  -- either a monadic or dyadic operator
            hGlbData = lptkFunc->tkData.tkSym->stData.stGlbData;

            break;          // Continue with common hGlbData code

        case TKT_FCNARRAY:
        case TKT_DELAFO:    // Del Anon -- either a function or a monadic or dyadic operator, bound to its operands
            hGlbData = lptkFunc->tkData.tkGlbData;

            break;          // Continue with common hGlbData code

        case TKT_SETALPHA:  // Set {alpha}
        defstop
            return -1;
    } // End SWITCH

    // stData/tkData is a valid HGLOBAL function array
    //   or user-defined function/operator
    Assert (IsGlbTypeFcnDir_PTB (hGlbData)
         || IsGlbTypeDfnDir_PTB (hGlbData));

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Split cases based upon the signature
    switch (GetSignatureMem (lpMem))
    {
#define lpHeader    ((LPFCNARRAY_HEADER) lpMem)
        case FCNARRAY_HEADER_SIGNATURE:
            fnNameType = lpHeader->fnNameType;

            break;
#undef  lpHeader

#define lpHeader    ((LPDFN_HEADER) lpMem)
        case DFN_HEADER_SIGNATURE:
            fnNameType = TranslateDfnTypeToNameType (lpHeader->DfnType, lpHeader->FcnValence);

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

void AssignArrayCommon
    (LPTOKEN     lptkNam,           // Ptr to name token
     LPTOKEN     lptkSrc,           // Ptr to source token
     TOKEN_TYPES TknType)           // Incoming token type for name (see TOKEN_TYPES)

{
    // Free the old value for this name
    FreeResultName (lptkNam);

////// Clear the name's STE flags
////ZeroMemory (&lptkNam->tkData.tkSym->stFlags,
////     sizeof (lptkNam->tkData.tkSym->stFlags));

    // Clear the UsrDfn and DfnAxis flags
    lptkNam->tkData.tkSym->stFlags.UsrDfn  =
    lptkNam->tkData.tkSym->stFlags.DfnAxis = FALSE;

    // Fill in the token type
    lptkNam->tkFlags.TknType = TknType;

    // Copy the HGLOBAL
    lptkNam->tkData.tkSym->stData.stGlbData = CopySymGlbDir_PTB (lptkSrc->tkData.tkGlbData);
} // End AssignArrayCommon


//***************************************************************************
//  $AssignNamedVars_EM
//
//  Assign values to a name strand
//***************************************************************************

UBOOL AssignNamedVars_EM
    (LPPL_YYSTYPE lpYYStr,      // Ptr to named strand
     LPPL_YYSTYPE lpYYVal)      // ...    value

{
    LPTOKEN           lptkStr,              // Ptr to named strand token
                      lptkVal;              // Ptr to value token
    UBOOL             bRet = TRUE;          // TRUE iff result is valid
    HGLOBAL           hGlbStr,              // Name strand global memory handle
                      hGlbVal = NULL,       // Value       ...
                      hGlbSub;              // Subarray    ...
    LPVARNAMED_HEADER lpMemHdrNam = NULL;   // Ptr to named strand header
    LPVARARRAY_HEADER lpMemHdrVal = NULL;   // ...    value        ...
    LPPL_YYSTYPE      lpMemNam;             // Ptr to name strand global memory
    LPVOID            lpMemVal;             // Ptr to value
    APLNELM           aplNELMNam,           // Name strand NELM
                      aplNELMVal,           // Value ...
                      aplName,              // Loop counter
                      aplIndex;             // Index into lpMemVal
    APLRANK           aplRankVal;           // Value rank
    APLSTYPE          aplTypeVal;           // Value storage type
    TOKEN             tkToken = {0};        // Temp token
    UINT              uBitMaskVal;          // Bit mask
    LPSYMENTRY        lpSymVal;             // Ptr to temp SYMENTRY
    APLINT            apaOffVal,            // APA offset
                      apaMulVal;            // ... multiplier
    ALLTYPES          atVal = {0};          // Value as ALLTYPES

    // Get ptrs to tokens
    lptkStr = &lpYYStr->tkToken;
    lptkVal = &lpYYVal->tkToken;

    Assert (lptkStr->tkFlags.TknType EQ TKT_STRNAMED);

    // Get the global handle
    hGlbStr = lptkStr->tkData.tkGlbData;

    // Lock the memory to get a ptr to it
    lpMemHdrNam = MyGlobalLockVnm (hGlbStr);

#define lpHeader    lpMemHdrNam
    // Get the # names in the strand
    aplNELMNam = lpHeader->NELM;
#undef  lpHeader

    // Skip over the name strand header to the data
    lpMemNam = VarNamedBaseToData (lpMemHdrNam);

    // If there's only one name, ...
    if (aplNELMNam EQ 1)
    {
        // Assign the entire value to the name
        // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
        AssignName_EM (&lpMemNam[0].tkToken, lptkVal);

        goto NORMAL_EXIT;
    } // End IF

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
                // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                AssignName_EM (&lpMemNam[aplName].tkToken, lptkVal);
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
    Assert (IsGlbTypeVarDir_PTB (hGlbVal));

    // Lock the memory to get a ptr to it
    lpMemHdrVal = MyGlobalLockVar (hGlbVal);

#define lpHeader    lpMemHdrVal
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
    lpMemVal = VarArrayDataFmBase (lpMemHdrVal);

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
////tkToken.tkFlags.NoDisplay   = FALSE;    // Already from from {0}
////tkToken.tkData              =
    tkToken.tkCharIndex         = lptkVal->tkCharIndex;

    // Split cases based upon the value storage type
    switch (aplTypeVal)
    {
        case ARRAY_BOOL:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_BOOL;

            uBitMaskVal = BIT0;

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMNam; aplName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkBoolean = (uBitMaskVal & *(LPAPLBOOL) lpMemVal) ? TRUE : FALSE;

                // Assign this token to this name
                // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                AssignName_EM (&lpMemNam[(aplNELMNam - 1) - aplName].tkToken, &tkToken);

                // If there's more than one value, ...
                if (!IsSingleton (aplNELMVal))
                {
                    // Shift over the bit mask
                    uBitMaskVal <<= 1;

                    // Check for end-of-byte
                    if (uBitMaskVal EQ END_OF_BYTE)
                    {
                        uBitMaskVal = BIT0;         // Start over
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
                // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                AssignName_EM (&lpMemNam[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
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
                // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                AssignName_EM (&lpMemNam[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
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
                // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                AssignName_EM (&lpMemNam[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
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
                // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                AssignName_EM (&lpMemNam[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_NESTED:
            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMNam; aplName++)
            {
                LPVOID lpVal;

                // Get the LPSYMENTRY or HGLOBAL
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
                        Assert (IsGlbTypeVarDir_PTB (hGlbSub));

                        // Fill in the value token
                        tkToken.tkFlags.TknType  = TKT_VARARRAY;
                        tkToken.tkFlags.ImmType  = IMMTYPE_ERROR;
                        tkToken.tkData.tkGlbData = hGlbSub;     // The call to AssignName_EM increments the RefCnt

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Assign this token to this name
                // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                AssignName_EM (&lpMemNam[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
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
                // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                AssignName_EM (&lpMemNam[(aplNELMNam - 1) - aplName].tkToken, &tkToken);
            } // End FOR

            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:

        case ARRAY_ARB:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Fill in the value token
            tkToken.tkFlags.TknType  = TKT_VARARRAY;
            tkToken.tkFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeVal);

            // Loop through the names/values
            for (aplName = 0; aplName < aplNELMNam; aplName++)
            {
                // Calculate the index
                aplIndex = aplName % aplNELMVal;

                // Copy to ALLTYPES
                (*aTypeActPromote[aplTypeVal][aplTypeVal]) (lpMemVal, aplIndex, &atVal);

                // Fill in the value token
                tkToken.tkData.tkGlbData =
                  MakeGlbEntry_EM (aplTypeVal,              // Entry type
                                  &atVal,                   // Ptr to the value
                                   TRUE,                    // TRUE iff we should initialize the target first
                                   lptkVal);                // Ptr to function token
                // Free the old atVal (if any)
                (*aTypeFree[aplTypeVal]) (&atVal, 0);

                // Assign this token to this name
                // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                AssignName_EM (&lpMemNam[(aplNELMNam - 1) - aplName].tkToken, &tkToken);

                DbgDecrRefCntTkn (&tkToken);    // EXAMPLE:  (a b c){is}1 2 3x
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
    if (hGlbVal NE NULL && lpMemHdrVal NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbVal); lpMemHdrVal = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbStr, lpMemHdrNam);

    // Mark as not displayable
    lptkVal->tkFlags.NoDisplay = TRUE;

    return bRet;
} // End AssignNamedVars_EM


//***************************************************************************
//  $ModifyAssignNamedVars_EM
//
//  Assign modified values to a name strand
//***************************************************************************

UBOOL ModifyAssignNamedVars_EM
    (LPPL_YYSTYPE  lpYYStrN,        // Ptr to name strand token
     LPPL_YYSTYPE  lpYYFcnStr,      // Ptr to function strand
     LPTOKEN       lptkVal)         // Ptr to value token

{
    HGLOBAL           hGlbName,             // Name strand global memory handle
                      hGlbVal = NULL,       // Value       ...
                      hGlbSub;              // Subarray    ...
    LPVARNAMED_HEADER lpMemHdrNam = NULL;   // Ptr to name  header
    LPVARARRAY_HEADER lpMemHdrVal = NULL;   // ...    value ...
    LPPL_YYSTYPE      lpMemNam,             // Ptr to name strand global memory
                      lpYYRes;              // Ptr to the result
    LPVOID            lpMemVal;             // Ptr to value
    APLNELM           aplNELMNam,           // Name strand NELM
                      aplNELMVal,           // Value ...
                      aplIndex;             // Index into lpMemVal
    APLUINT           uName;                // Loop counter
    APLRANK           aplRankVal;           // Value rank
    APLSTYPE          aplTypeVal;           // Value storage type
    TOKEN             tkToken = {0};        // Temp token
    UINT              uBitMaskVal;          // Bool mask
    LPSYMENTRY        lpSymVal;             // Ptr to global memory
    APLINT            apaOffVal,            // APA offset
                      apaMulVal;            // ... multiplier
    ALLTYPES          atVal = {0};          // Value as ALLTYPES
    UBOOL             bRet = FALSE;         // TRUE iff result is valid

    // Get the name strand global memory handle
    hGlbName = lpYYStrN->tkToken.tkData.tkGlbData;

    // tkData is a valid HGLOBAL name strand
    Assert (IsGlbTypeNamDir_PTB (hGlbName));

    // Lock the memory to get a ptr to it
    lpMemHdrNam = MyGlobalLockVnm (hGlbName);

#define lpHeader        lpMemHdrNam
    aplNELMNam = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header to the data
    lpMemNam = VarNamedBaseToData (lpMemHdrNam);

    // If there's only one name, ...
    if (aplNELMNam EQ 1)
    {
        lpYYRes =
          ExecFunc_EM_YY (&lpMemNam[0].tkToken, lpYYFcnStr, lptkVal);

        if (lpYYRes)
        {
            // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
            //   and increments the RefCnt
            bRet = AssignName_EM (&lpMemNam[0].tkToken, &lpYYRes->tkToken);
            FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

            if (!bRet)
                goto ERROR_EXIT;
        } else
            goto ERROR_EXIT;
        goto NORMAL_EXIT;
    } // End IF

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
            for (uName = 0; uName < aplNELMNam; uName++)
            {
                // Execute the function between the named value and the immediate arg value
                lpYYRes =
                  ExecFunc_EM_YY (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, lpYYFcnStr, lptkVal);

                if (lpYYRes)
                {
                    // Assign this token to this name
                    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                    //   and increments the RefCnt
                    bRet = AssignName_EM (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, &lpYYRes->tkToken);
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } else
                    goto ERROR_EXIT;
            } // End FOR

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
    Assert (IsGlbTypeVarDir_PTB (hGlbVal));

    // Lock the memory to get a ptr to it
    lpMemHdrVal = MyGlobalLockVar (hGlbVal);

#define lpHeader    lpMemHdrVal
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
    lpMemVal = VarArrayDataFmBase (lpMemHdrVal);

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
////tkToken.tkFlags.NoDisplay   = FALSE;    // Already from from {0}
////tkToken.tkData              =
    tkToken.tkCharIndex         = lptkVal->tkCharIndex;

    // Split cases based upon the value storage type
    switch (aplTypeVal)
    {
        case ARRAY_BOOL:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_BOOL;

            uBitMaskVal = BIT0;

            // Loop through the names/values backwards
            for (uName = 0; uName < aplNELMNam; uName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkBoolean = (uBitMaskVal & *(LPAPLBOOL) lpMemVal) ? TRUE : FALSE;

                // Execute the function between the named value and the arg value
                lpYYRes =
                  ExecFunc_EM_YY (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, lpYYFcnStr, &tkToken);

                if (lpYYRes)
                {
                    // Assign this token to this name
                    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                    //   and increments the RefCnt
                    bRet = AssignName_EM (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, &lpYYRes->tkToken);
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } else
                    goto ERROR_EXIT;

                // If there's more than one value, ...
                if (!IsSingleton (aplNELMVal))
                {
                    // Shift over the bit mask
                    uBitMaskVal <<= 1;

                    // Check for end-of-byte
                    if (uBitMaskVal EQ END_OF_BYTE)
                    {
                        uBitMaskVal = BIT0;         // Start over
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
            for (uName = 0; uName < aplNELMNam; uName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkInteger = ((LPAPLINT) lpMemVal)[uName % aplNELMVal];

                // Execute the function between the named value and the arg value
                lpYYRes =
                  ExecFunc_EM_YY (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, lpYYFcnStr, &tkToken);

                if (lpYYRes)
                {
                    // Assign this token to this name
                    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                    //   and increments the RefCnt
                    bRet = AssignName_EM (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, &lpYYRes->tkToken);
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } else
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_FLOAT:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_FLOAT;

            // Loop through the names/values
            for (uName = 0; uName < aplNELMNam; uName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkFloat = ((LPAPLFLOAT) lpMemVal)[uName % aplNELMVal];

                // Execute the function between the named value and the arg value
                lpYYRes =
                  ExecFunc_EM_YY (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, lpYYFcnStr, &tkToken);

                if (lpYYRes)
                {
                    // Assign this token to this name
                    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                    //   and increments the RefCnt
                    bRet = AssignName_EM (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, &lpYYRes->tkToken);
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } else
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_CHAR;

            // Loop through the names/values
            for (uName = 0; uName < aplNELMNam; uName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkChar = ((LPAPLCHAR) lpMemVal)[uName % aplNELMVal];

                // Execute the function between the named value and the arg value
                lpYYRes =
                  ExecFunc_EM_YY (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, lpYYFcnStr, &tkToken);

                if (lpYYRes)
                {
                    // Assign this token to this name
                    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                    //   and increments the RefCnt
                    bRet = AssignName_EM (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, &lpYYRes->tkToken);
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } else
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_HETERO:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;

            // Loop through the names/values
            for (uName = 0; uName < aplNELMNam; uName++)
            {
                // Get a ptr to the next value
                lpSymVal = ((LPAPLHETERO) lpMemVal)[uName % aplNELMVal];

                // It's an LPSYMENTRY
                Assert (GetPtrTypeDir (lpSymVal) EQ PTRTYPE_STCONST);

                // It's an immediate
                Assert (lpSymVal->stFlags.Imm);

                // Save the immediate type and value in the token
                tkToken.tkFlags.ImmType = lpSymVal->stFlags.ImmType;
                tkToken.tkData.tkLongest = lpSymVal->stData.stLongest;

                // Execute the function between the named value and the arg value
                lpYYRes =
                  ExecFunc_EM_YY (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, lpYYFcnStr, &tkToken);

                if (lpYYRes)
                {
                    // Assign this token to this name
                    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                    //   and increments the RefCnt
                    bRet = AssignName_EM (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, &lpYYRes->tkToken);
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } else
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_NESTED:
            // Loop through the names/values
            for (uName = 0; uName < aplNELMNam; uName++)
            {
                LPVOID lpVal;

                // Get the LPSYMENTRY or HGLOBAL
                lpVal = ((LPAPLNESTED) lpMemVal)[uName % aplNELMVal];

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
                        Assert (IsGlbTypeVarDir_PTB (hGlbSub));

                        // Fill in the value token
                        tkToken.tkFlags.TknType  = TKT_VARARRAY;
                        tkToken.tkFlags.ImmType  = IMMTYPE_ERROR;
                        tkToken.tkData.tkGlbData = hGlbSub;     // The call to AssignName_EM increments the RefCnt

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Execute the function between the named value and the arg value
                lpYYRes =
                  ExecFunc_EM_YY (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, lpYYFcnStr, &tkToken);

                if (lpYYRes)
                {
                    // Assign this token to this name
                    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                    //   and increments the RefCnt
                    bRet = AssignName_EM (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, &lpYYRes->tkToken);
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } else
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_APA:
            // Fill in the value token
            tkToken.tkFlags.TknType = TKT_VARIMMED;
            tkToken.tkFlags.ImmType = IMMTYPE_INT;

            // Loop through the names/values
            for (uName = 0; uName < aplNELMNam; uName++)
            {
                // Save the next value into the token
                tkToken.tkData.tkInteger = apaOffVal + apaMulVal * (uName % aplNELMVal);

                // Execute the function between the named value and the arg value
                lpYYRes =
                  ExecFunc_EM_YY (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, lpYYFcnStr, &tkToken);

                if (lpYYRes)
                {
                    // Assign this token to this name
                    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                    //   and increments the RefCnt
                    bRet = AssignName_EM (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, &lpYYRes->tkToken);
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } else
                    goto ERROR_EXIT;
            } // End FOR

            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:

        case ARRAY_ARB:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Fill in the value token
            tkToken.tkFlags.TknType  = TKT_VARARRAY;
            tkToken.tkFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeVal);

            // Loop through the names/values
            for (uName = 0; uName < aplNELMNam; uName++)
            {
                // Calculate the index
                aplIndex = uName % aplNELMVal;

                // Copy to ALLTYPES
                (*aTypeActPromote[aplTypeVal][aplTypeVal]) (lpMemVal, aplIndex, &atVal);

                // Free the old value
                (*aTypeFree[aplTypeVal]) (lpMemVal, aplIndex);

                // Fill in the value token
                tkToken.tkData.tkGlbData =
                  MakeGlbEntry_EM (aplTypeVal,              // Entry type
                                  &atVal,                   // Ptr to the value
                                   TRUE,                    // TRUE iff we should initialize the target first
                                   lptkVal);                // Ptr to function token
                // Free the old atVal (if any)
                (*aTypeFree[aplTypeVal]) (&atVal, 0);

                // Execute the function between the named value and the arg value
                lpYYRes =
                  ExecFunc_EM_YY (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, lpYYFcnStr, &tkToken);

                if (lpYYRes)
                {
                    // Assign this token to this name
                    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
                    //   and increments the RefCnt
                    bRet = AssignName_EM (&lpMemNam[(aplNELMNam - 1) - uName].tkToken, &lpYYRes->tkToken);
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                    if (!bRet)
                        goto ERROR_EXIT;
                } else
                    goto ERROR_EXIT;
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

NORMAL_EXIT:
    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbName); lpMemHdrNam = NULL;

    return bRet;
} // End ModifyAssignNamedVars_EM


//***************************************************************************
//  End of File: assign.c
//***************************************************************************
