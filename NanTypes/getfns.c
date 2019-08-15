//***************************************************************************
//  NARS2000 -- Getxxx Functions
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

static LPVOID Ptr2Null = NULL;


//***************************************************************************
//  $GetPtrTknLongest
//
//  Return a ptr to either the tkLongest or stLongest of a token
//***************************************************************************

LPAPLLONGEST GetPtrTknLongest
    (LPTOKEN lptkToken)                 // Ptr to the token

{
    // If it's named, ...
    if (IsTknNamed (lptkToken))
        // Get a ptr to the stLongest value from the SYMENTRY
        return &lptkToken->tkData.tkSym->stData.stLongest;
    else
        // Get a ptr to the tkLongest value from the TOKEN
        return &lptkToken->tkData.tkLongest;
} // End GetPtrTknLongest


//***************************************************************************
//  $GetPtrGlbDataToken
//
//  Return the xxGlbData value from a token
//    taking into account whether or the token is named
//***************************************************************************

HGLOBAL *GetPtrGlbDataToken
    (LPTOKEN lptkToken)                 // Ptr to the token

{
    // If it's named, ...
    if (IsTknNamed (lptkToken))
        // Get the xxGlbData value from the SYMENTRY
        return &lptkToken->tkData.tkSym->stData.stGlbData;
    else
    if (!IsTknImmed (lptkToken))
        // Get the xxGlbData value from the TOKEN
        return &lptkToken->tkData.tkGlbData;
    else
        return &Ptr2Null;
} // End GetPtrGlbDataToken


//***************************************************************************
//  $GetGlbDataToken
//
//  Return the xxGlbData value from a token
//    taking into account whether or the token is named
//***************************************************************************

HGLOBAL GetGlbDataToken
    (LPTOKEN lptkToken)                 // Ptr to the token

{
    return *GetPtrGlbDataToken (lptkToken);
} // End GetGlbDataToken


//***************************************************************************
//  $SetGlbDataToken
//
//  Return the xxGlbData value from a token
//    taking into account whether or the token is named
//***************************************************************************

UBOOL SetGlbDataToken
    (LPTOKEN lptkToken,                 // Ptr to the token
     HGLOBAL hGlbSrc)                   // Source global memory handle

{
    *GetPtrGlbDataToken (lptkToken) = MakePtrTypeGlb (hGlbSrc);

    return TRUE;
} // End SetGlbDataToken


//***************************************************************************
//  $ValidateFirstItemToken
//
//  Get and validate the first item from the token as an integer
//***************************************************************************

APLLONGEST ValidateFirstItemToken
    (APLSTYPE aplTypeRht,               // Right arg storage type
     LPTOKEN  lpToken,                  // Ptr to the token
     LPUBOOL  lpbRet)                   // Ptr to TRUE iff the result is valid

{
    APLLONGEST aplLongestRht;           // Right arg longest if immediate
    HGLOBAL    hGlbSubRht;              // Right arg item global memory handle
    int        iHCDimRht,               // HC Dimension (1, 2, 4, 8)
               i;                       // Loop counter

    GetFirstItemToken (lpToken,         // Ptr to the token
                      &aplLongestRht,   // Ptr to the longest (may be NULL)
                       NULL,            // ...        immediate type (see IMM_TYPES) (may be NULL)
                      &hGlbSubRht);     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Mark as successful
            *lpbRet = TRUE;

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, lpbRet);

            break;

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
            Assert (GetPtrTypeDir (hGlbSubRht) NE PTRTYPE_HGLOBAL);

            // Attempt to convert the HCxy to an integer using System []CT
            aplLongestRht = ConvertToInteger_SCT (aplTypeRht, hGlbSubRht, 0, lpbRet);

            // Calculate the HC Dimension (1, 2, 4, 8)
            iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

            // If that worked, ...
            if (*lpbRet)
            // Loop through the imaginary parts
            for (i = 1; *lpbRet && i < iHCDimRht; i++)
                // If this imaginary part is not zero, ...
                if (ConvertToInteger_SCT (aplTypeRht, &hGlbSubRht, i, lpbRet) NE 0)
                    // Mark as invalid
                    *lpbRet = FALSE;
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Mark as invalid
            *lpbRet = FALSE;

            break;

        defstop
            // Mark as invalid
            *lpbRet = FALSE;

            break;
    } // End SWITCH

    return aplLongestRht;
} // End ValidateFirstItemToken


//***************************************************************************
//  $GetNextIntegerToken
//
//  Return the next value from a token as an integer.
//***************************************************************************

APLINT GetNextIntegerToken
    (LPTOKEN  lptkArg,                      // Ptr to arg token
     APLINT   uIndex,                       // Index
     APLSTYPE aplTypeArg,                   // Arg storage type
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    APLLONGEST        aplLongestArg;        // Immediate value
    HGLOBAL           hGlbArg = NULL;       // Right arg global memory handle
    LPVARARRAY_HEADER lpMemHdrArg = NULL;   // Ptr to memory header
    LPVOID            lpMemArg;             // Ptr to right arg global memory
    APLINT            aplIntegerRes;        // The result
    APLSTYPE          aplTypeItm;           // Item storage type
    APLFLOAT          aplFloatItm;          // Immediate item as a float
    APLCHAR           aplCharItm;           // Immediate item as a char
    HGLOBAL           hGlbItm;              // ...                 HGLOBAL

    // Assume it'll be successful
    *lpbRet = TRUE;

    // If the arg is simple non-hetero, ...
    if (IsSimpleNH (aplTypeArg))
        // Get the next value from the right operand (index origin)
        GetNextValueToken (lptkArg,                     // Ptr to the token
                           uIndex,                      // Index to use
                           NULL,                        // Ptr to the integer (or Boolean) (may be NULL)
                           NULL,                        // ...        float (may be NULL)
                           NULL,                        // ...        char (may be NULL)
                          &aplLongestArg,               // ...        longest (may be NULL)
                           NULL,                        // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                           NULL,                        // ...        immediate type (see IMM_TYPES) (may be NULL)
                           NULL);                       // ...        array type:  ARRAY_TYPES (may be NULL)
    else
    // Otherwise it's global numeric
    {
        // Get the global ptrs
        GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemHdrArg);

        // If it's not an immediate, ...
        if (hGlbArg NE NULL)
            lpMemArg = VarArrayDataFmBase (lpMemHdrArg);
    } // End IF/ELSE

    // Split cases based upon the storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            aplIntegerRes = aplLongestArg;

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplIntegerRes = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestArg, lpbRet);

            break;

        case ARRAY_RAT:
            aplIntegerRes = GetNextRatIntMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_VFP:
            aplIntegerRes = GetNextVfpIntMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_ARB:
            aplIntegerRes = GetNextArbIntMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_HETERO:
            // Get the next values and type
            aplTypeItm = GetNextHetero (lpMemArg, uIndex, &aplIntegerRes, &aplFloatItm, &aplCharItm, &hGlbItm);

            // Split cases based upon the storage type
            switch (aplTypeItm)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                    goto NORMAL_EXIT;

                case ARRAY_FLOAT:
                    // Attempt to convert the float to an integer using System []CT
                    aplIntegerRes = FloatToAplint_SCT (aplFloatItm, lpbRet);

                    goto NORMAL_EXIT;

                case ARRAY_RAT:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the RAT to an integer using System []CT
                    aplIntegerRes = mpq_get_sctsx ((LPAPLRAT) hGlbItm, lpbRet);

                    goto NORMAL_EXIT;

                case ARRAY_VFP:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the VFP to an integer using System []CT
                    aplIntegerRes = mpfr_get_sctsx ((LPAPLVFP) hGlbItm, lpbRet);

                    goto NORMAL_EXIT;

                case ARRAY_ARB:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the ARB to an integer using System []CT
                    aplIntegerRes = arb_get_sctsx ((LPAPLARB) hGlbItm, lpbRet);

                    goto NORMAL_EXIT;

                case ARRAY_HC2I:
                case ARRAY_HC4I:
                case ARRAY_HC8I:
                case ARRAY_HC2F:
                case ARRAY_HC4F:
                case ARRAY_HC8F:
                case ARRAY_HC2R:
                case ARRAY_HC4R:
                case ARRAY_HC8R:
                case ARRAY_HC2V:
                case ARRAY_HC4V:
                case ARRAY_HC8V:
                case ARRAY_BA2F:
                case ARRAY_BA4F:
                case ARRAY_BA8F:
                    // Attempt to convert the item to an integer using System []CT
                    aplIntegerRes = ConvertToInteger_SCT (aplTypeItm, hGlbItm, 0, lpbRet);

                    goto NORMAL_EXIT;

                case ARRAY_CHAR:
                case ARRAY_NESTED:
                    break;

                case ARRAY_APA:             // Can't happen
                case ARRAY_HETERO:          // ...
                defstop
                    break;
            } // End SWITCH

            // Fall through to common error code

        case ARRAY_CHAR:
        case ARRAY_NESTED:
            // Mark as in error
            *lpbRet = FALSE;

            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Attempt to convert the item to an integer using System []CT
            aplIntegerRes = ConvertToInteger_SCT (aplTypeArg, lpMemArg, uIndex, lpbRet);

            break;

        defstop
            break;
    } // End SWITCH
NORMAL_EXIT:
    // If it's not an immediate, ...
    if (hGlbArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;
    } // End IF

    return aplIntegerRes;
} // End GetNextIntegerToken


//***************************************************************************
//  $GetNextFloatToken
//
//  Return the next value from a token as a float.
//***************************************************************************

APLFLOAT GetNextFloatToken
    (LPTOKEN  lptkArg,                      // Ptr to arg token
     APLINT   uIndex,                       // Index
     APLSTYPE aplTypeArg,                   // Arg storage type
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    APLLONGEST        aplLongestArg;        // Immediate value
    HGLOBAL           hGlbArg = NULL;       // Right arg global memory handle
    LPVARARRAY_HEADER lpMemHdrArg = NULL;   // Ptr to memory header
    LPVOID            lpMemArg;             // Ptr to right arg global memory
    APLINT            aplIntegerItm;        // Immediate value as integer
    APLSTYPE          aplTypeItm;           // Item storage type
    APLFLOAT          aplFloatRes;          // The result
    APLCHAR           aplCharItm;           // Immediate item as a char
    HGLOBAL           hGlbItm;              // ...                 HGLOBAL

    // Assume it'll be successful
    *lpbRet = TRUE;

    // If the arg is simple non-hetero, ...
    if (IsSimpleNH (aplTypeArg))
        // Get the next value from the right operand (index origin)
        GetNextValueToken (lptkArg,                     // Ptr to the token
                           uIndex,                      // Index to use
                           NULL,                        // Ptr to the integer (or Boolean) (may be NULL)
                           NULL,                        // ...        float (may be NULL)
                           NULL,                        // ...        char (may be NULL)
                          &aplLongestArg,               // ...        longest (may be NULL)
                           NULL,                        // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                           NULL,                        // ...        immediate type (see IMM_TYPES) (may be NULL)
                           NULL);                       // ...        array type:  ARRAY_TYPES (may be NULL)
    else
    // Otherwise it's global numeric
    {
        // Get the global ptrs
        GetGlbPtrs_LOCK (lptkArg, &hGlbArg, &lpMemHdrArg);

        // If it's not an immediate, ...
        if (hGlbArg NE NULL)
            lpMemArg = VarArrayDataFmBase (lpMemHdrArg);
    } // End IF/ELSE

    // Split cases based upon the storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            aplFloatRes = (APLFLOAT) aplLongestArg;

            break;

        case ARRAY_FLOAT:
            aplFloatRes = *(LPAPLFLOAT) &aplLongestArg;

            break;

        case ARRAY_RAT:
            aplFloatRes = GetNextRatFltMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_VFP:
            aplFloatRes = GetNextVfpFltMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_ARB:
            aplFloatRes = GetNextArbFltMem (lpMemArg, uIndex, lpbRet);

            break;

        case ARRAY_HETERO:
            // Get the next values and type
            aplTypeItm = GetNextHetero (lpMemArg, uIndex, &aplIntegerItm, &aplFloatRes, &aplCharItm, &hGlbItm);

            // Split cases based upon the storage type
            switch (aplTypeItm)
            {
                case ARRAY_BOOL:            // GetNextHetero already converts INT to FLOAT
                case ARRAY_INT:             // ...
                case ARRAY_FLOAT:
                    goto NORMAL_EXIT;

                case ARRAY_RAT:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the RAT to a float
                    aplFloatRes = mpq_get_d ((LPAPLRAT) hGlbItm);

                    goto NORMAL_EXIT;

                case ARRAY_VFP:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the VFP to a float
                    aplFloatRes = mpfr_get_d ((LPAPLVFP) hGlbItm, MPFR_RNDN);

                    goto NORMAL_EXIT;

                case ARRAY_ARB:
                    Assert (GetPtrTypeDir (hGlbItm) NE PTRTYPE_HGLOBAL);

                    // Attempt to convert the ARB to a float
                    aplFloatRes = arb_get_d ((LPAPLARB) hGlbItm);

                    goto NORMAL_EXIT;

                case ARRAY_CHAR:
                case ARRAY_NESTED:
                    break;

                case ARRAY_HC2I:
                case ARRAY_HC4I:
                case ARRAY_HC8I:
                case ARRAY_HC2F:
                case ARRAY_HC4F:
                case ARRAY_HC8F:
                case ARRAY_HC2R:
                case ARRAY_HC4R:
                case ARRAY_HC8R:
                case ARRAY_HC2V:
                case ARRAY_HC4V:
                case ARRAY_HC8V:
                case ARRAY_BA2F:
                case ARRAY_BA4F:
                case ARRAY_BA8F:
                    // Attempt to convert the item to a float
                    aplFloatRes = ConvertToFloat (aplTypeItm, hGlbItm, 0, lpbRet);

                    goto NORMAL_EXIT;

                case ARRAY_APA:             // Can't happen
                case ARRAY_HETERO:          // ...
                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_CHAR:
        case ARRAY_NESTED:
            // Mark as in error
            *lpbRet = FALSE;

            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Attempt to convert the item to a float
            aplFloatRes = ConvertToFloat (aplTypeArg, lpMemArg, 0, lpbRet);

            break;

        defstop
            break;
    } // End SWITCH
NORMAL_EXIT:
    // If it's not an immediate, ...
    if (hGlbArg NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdrArg = NULL;
    } // End IF

    return aplFloatRes;
} // End GetNextFloatToken


//***************************************************************************
//  $GetFirstItemToken
//
//  Return the first item from the token
//***************************************************************************

void GetFirstItemToken
    (LPTOKEN      lpToken,          // Ptr to the token
     LPAPLLONGEST lpaplLongest,     // Ptr to the longest (may be NULL)
     LPIMM_TYPES  lpImmType,        // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPVOID      *lpSymGlb)         // ...        LPSYMENTRY or HGLOBAL (may be NULL)

{
    HGLOBAL hGlbData;               // Global memory handle of TKT_VARNAMED or TKT_VARARRAY

    // Fill in default values
    if (lpSymGlb NE NULL)
        *lpSymGlb = NULL;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                              lpToken->tkData.tkSym->stData.stLongest,
                              NULL,
                              NULL,
                              NULL,
                              lpaplLongest,
                              lpImmType,
                              NULL);
            return;

        case TKT_VARIMMED:
            // Handle the immediate case
            GetFirstValueImm (lpToken->tkFlags.ImmType,
                              lpToken->tkData.tkLongest,
                              NULL,
                              NULL,
                              NULL,
                              lpaplLongest,
                              lpImmType,
                              NULL);
            return;

        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetFirstValueGlb (hGlbData,         // The global memory handle
                      NULL,             // Ptr to integer (or Boolean) (may be NULL)
                      NULL,             // ...    float (may be NULL)
                      NULL,             // ...    char (may be NULL)
                      lpaplLongest,     // ...    longest (may be NULL)
                      lpSymGlb,         // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                      lpImmType,        // ...    immediate type (see IMM_TYPES) (may be NULL)
                      NULL,             // ...    array type -- ARRAY_TYPES (may be NULL)
                      FALSE);           // TRUE iff we should expand LPSYMENTRY into immediate value
} // End GetFirstItemToken


//***************************************************************************
//  $GetNextValueTokenIntoToken
//
//  Return the next value from a token as either
//    a VARIMMED or VARARRAY.
//  The token may be an empty array in which case the
//    value of the prototype is returned.
//***************************************************************************

void GetNextValueTokenIntoToken
    (LPTOKEN      lptkArg,      // Ptr to the arg token
     APLUINT      uIndex,       // Index to use
     LPTOKEN      lptkRes)      // Ptr to the result token

{
    HGLOBAL    hGlbData,
               hGlbSub;
    IMM_TYPES  immType;

    // Fill in common values
    lptkRes->tkFlags.NoDisplay = FALSE;
    lptkRes->tkCharIndex       = lptkArg->tkCharIndex;
    lptkRes->tkSynObj          = soA;

    // Split cases based upon the token type
    switch (lptkArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lptkArg->tkData.tkSym->stFlags.ImmType,
                              lptkArg->tkData.tkSym->stData.stLongest,
                              NULL,
                              NULL,
                              NULL,
                             &lptkRes->tkData.tkLongest,
                             &immType,
                              NULL);
            // Fill in the result token
            lptkRes->tkFlags.TknType   = TKT_VARIMMED;
            lptkRes->tkFlags.ImmType   = immType;
////////////lptkRes->tkFlags.NoDisplay =                    // Filled in above
////////////lptkRes->tkData.tkLongest  =                    // Filled in above
////////////lptkRes->tkCharIndex       =                    // Filled in above

            return;

        case TKT_VARIMMED:
            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lptkArg->tkFlags.ImmType,
                              lptkArg->tkData.tkLongest,
                              NULL,
                              NULL,
                              NULL,
                             &lptkRes->tkData.tkLongest,
                             &immType,
                              NULL);
            // Fill in the result token
            lptkRes->tkFlags.TknType   = TKT_VARIMMED;
            lptkRes->tkFlags.ImmType   = immType;
////////////lptkRes->tkFlags.NoDisplay =                    // Filled in above
////////////lptkRes->tkData.tkLongest  =                    // Filled in above
////////////lptkRes->tkCharIndex       =                    // Filled in above

            return;

        case TKT_VARARRAY:
            hGlbData = lptkArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetNextValueGlb (hGlbData,                          // The global memory handle
                     uIndex,                            // Index into item
                    &hGlbSub,                           // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    &lptkRes->tkData.tkLongest,         // Ptr to result immediate value (may be NULL)
                    &immType);                          // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // If the return value is immediate, ...
    if (hGlbSub EQ NULL)
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType   = TKT_VARIMMED;
        lptkRes->tkFlags.ImmType   = immType;
////////lptkRes->tkFlags.NoDisplay =                    // Filled in above
////////lptkRes->tkData.tkLongest  =                    // Filled in above
////////lptkRes->tkCharIndex       =                    // Filled in above
    } else
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType   = TKT_VARARRAY;
        lptkRes->tkFlags.ImmType   = immType;
////////lptkRes->tkFlags.NoDisplay =                    // Filled in above
        lptkRes->tkData.tkGlbData  = hGlbSub;
////////lptkRes->tkCharIndex       =                    // Filled in above
    } // End IF/ELSE
} // End GetNextValueTokenIntoToken


//***************************************************************************
//  $GetNextValueTokenIntoNamedVarToken_EM
//
//  Return the next value from a token as a VARNAMED.
//  The token may be an empty array in which case the
//    value of the prototype is returned.
//  Note that the nested global case increments the reference count
//***************************************************************************

UBOOL GetNextValueTokenIntoNamedVarToken_EM
    (LPTOKEN      lptkArg,      // Ptr to the arg token
     APLUINT      uIndex,       // Index to use
     LPTOKEN      lptkRes)      // Ptr to the result token

{
    HGLOBAL           hGlbData,         // HGLOBAL of data in the arg
                      hGlbSub;          // ...        indexed item in the arg
    IMM_TYPES         immType;          // Immediate type
    UBOOL             bRet = TRUE;      // TRUE iff the result is valid

    // tkData is an LPSYMENTRY
    Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

    // Free the previous value (if any)
    FreeResultName (lptkRes);

    // Fill in common values
    lptkRes->tkFlags.ImmType   = IMMTYPE_ERROR;
    lptkRes->tkFlags.NoDisplay = FALSE;
    lptkRes->tkCharIndex       = lptkArg->tkCharIndex;

    // Split cases based upon the token type
    switch (lptkArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkArg->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lptkArg->tkData.tkSym->stFlags.ImmType,
                              lptkArg->tkData.tkSym->stData.stLongest,
                              NULL,
                              NULL,
                              NULL,
                             &lptkRes->tkData.tkSym->stData.stLongest,
                             &immType,
                              NULL);
            // Fill in the result token
            lptkRes->tkFlags.TknType                  = TKT_VARNAMED;
////////////lptkRes->tkFlags.ImmType                  =     // Filled in above
////////////lptkRes->tkFlags.NoDisplay                =     // Filled in above
            lptkRes->tkData.tkSym->stFlags.Imm        = TRUE;
            lptkRes->tkData.tkSym->stFlags.ImmType    = immType;
            lptkRes->tkData.tkSym->stFlags.Value      = TRUE;
            lptkRes->tkData.tkSym->stFlags.ObjName    = OBJNAME_USR;
            lptkRes->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;
////////////lptkRes->tkData.tkSym->stData.stLongest   =     // Filled in above
////////////lptkRes->tkCharIndex                      =     // Filled in above

            goto NORMAL_EXIT;

        case TKT_VARIMMED:
            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lptkArg->tkFlags.ImmType,
                              lptkArg->tkData.tkLongest,
                              NULL,
                              NULL,
                              NULL,
                             &lptkRes->tkData.tkSym->stData.stLongest,
                             &immType,
                              NULL);
            // Fill in the result token
            lptkRes->tkFlags.TknType                  = TKT_VARNAMED;
////////////lptkRes->tkFlags.ImmType                  =     // Filled in above
////////////lptkRes->tkFlags.NoDisplay                =     // Filled in above
            lptkRes->tkData.tkSym->stFlags.Imm        = TRUE;
            lptkRes->tkData.tkSym->stFlags.ImmType    = immType;
            lptkRes->tkData.tkSym->stFlags.Value      = TRUE;
            lptkRes->tkData.tkSym->stFlags.ObjName    = OBJNAME_USR;
            lptkRes->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;
////////////lptkRes->tkData.tkSym->stData.stLongest   =     // Filled in above
////////////lptkRes->tkCharIndex                      =     // Filled in above

            goto NORMAL_EXIT;

        case TKT_VARARRAY:
            hGlbData = lptkArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            goto NORMAL_EXIT;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetNextValueGlb (hGlbData,                              // The global memory handle
                     uIndex,                                // Index into item
                    &hGlbSub,                               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    &lptkRes->tkData.tkSym->stData.stLongest,// Ptr to result immediate value (may be NULL)
                    &immType);                              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // If the return value is immediate, ...
    if (hGlbSub EQ NULL)
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType                  = TKT_VARNAMED;
////////lptkRes->tkFlags.ImmType                  =     // Filled in above
////////lptkRes->tkFlags.NoDisplay                =     // Filled in above
        lptkRes->tkData.tkSym->stFlags.Imm        = TRUE;
        lptkRes->tkData.tkSym->stFlags.ImmType    = immType;
        lptkRes->tkData.tkSym->stFlags.Value      = TRUE;
        lptkRes->tkData.tkSym->stFlags.ObjName    = OBJNAME_USR;
        lptkRes->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;
////////lptkRes->tkData.tkSym->stData.stLongest   =     // Filled in above
////////lptkRes->tkCharIndex                      =     // Filled in above
    } else
    {
        // Fill in the result token
        lptkRes->tkFlags.TknType                  = TKT_VARNAMED;
////////lptkRes->tkFlags.ImmType                  =     // Filled in above
////////lptkRes->tkFlags.NoDisplay                =     // Filled in above
        lptkRes->tkData.tkSym->stFlags.Imm        = FALSE;
        lptkRes->tkData.tkSym->stFlags.ImmType    = immType;
        lptkRes->tkData.tkSym->stFlags.Value      = TRUE;
        lptkRes->tkData.tkSym->stFlags.ObjName    = OBJNAME_USR;
        lptkRes->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;
////////lptkRes->tkData.tkSym->stData.stGlbData   =     // Filled in below
////////lptkRes->tkCharIndex                      =     // Filled in above

        // Split cases based upon the immediate type
        switch (immType)
        {
            case IMMTYPE_BOOL:
            case IMMTYPE_INT:
            case IMMTYPE_FLOAT:
            case IMMTYPE_CHAR:
            case IMMTYPE_ERROR:         // For ARRAY_NESTED
                lptkRes->tkData.tkSym->stData.stGlbData   = CopySymGlbDir_PTB (hGlbSub);

                break;

            case IMMTYPE_RAT:
            case IMMTYPE_VFP:
            case IMMTYPE_ARB:
            case IMMTYPE_HC2I:
            case IMMTYPE_HC2F:
            case IMMTYPE_HC2R:
            case IMMTYPE_HC2V:
            case IMMTYPE_HC4I:
            case IMMTYPE_HC4F:
            case IMMTYPE_HC4R:
            case IMMTYPE_HC4V:
            case IMMTYPE_HC8I:
            case IMMTYPE_HC8F:
            case IMMTYPE_HC8R:
            case IMMTYPE_HC8V:
            case IMMTYPE_BA2F:
            case IMMTYPE_BA4F:
            case IMMTYPE_BA8F:
                // Split cases based upon the ptr type
                switch (GetPtrTypeDir (hGlbSub))
                {
                    case PTRTYPE_STCONST:
                        lptkRes->tkData.tkSym->stData.stGlbData =
                          MakeGlbEntry_EM (TranslateImmTypeToArrayType (immType),   // Entry type
                                           hGlbSub,                                 // Ptr to the value
                                           TRUE,                                    // TRUE iff we should initialize the target first
                                           lptkArg);                                // Ptr to function token
                        break;

                    case PTRTYPE_HGLOBAL:
                        lptkRes->tkData.tkSym->stData.stGlbData = CopySymGlbDir_PTB (hGlbSub);

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            defstop
                break;
        } // End SWITCH

        // Check for error
        bRet = (lptkRes->tkData.tkSym->stData.stGlbData NE NULL);
    } // End IF/ELSE
NORMAL_EXIT:
    return bRet;
} // End GetNextValueTokenIntoNamedVarToken_EM


//***************************************************************************
//  $GetNextValueToken
//
//  Return the next value from a token as either
//    both an integer and a float, or as a character,
//    or as an LPSYMENTRY/HGLOBAL.  The token may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void GetNextValueToken
    (LPTOKEN      lpToken,      // Ptr to the token
     APLUINT      uIndex,       // Index to use
     LPAPLINT     lpaplInteger, // Ptr to the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type:  ARRAY_TYPES (may be NULL)

{
    HGLOBAL    hGlbData,
               hGlbSub;
    APLLONGEST aplLongest;
    IMM_TYPES  immType;

    // Fill in default values
    if (lpSymGlb NE NULL)
        *lpSymGlb = NULL;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                              lpToken->tkData.tkSym->stData.stLongest,
                              lpaplInteger,
                              lpaplFloat,
                              lpaplChar,
                              lpaplLongest,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_VARIMMED:
            Assert (uIndex EQ 0);

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkFlags.ImmType,
                              lpToken->tkData.tkLongest,
                              lpaplInteger,
                              lpaplFloat,
                              lpaplChar,
                              lpaplLongest,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetNextValueGlb (hGlbData,      // The global memory handle
                     uIndex,        // Index into item
                    &hGlbSub,       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    &aplLongest,    // Ptr to result immediate value (may be NULL)
                    &immType);      // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // Fill in various result values
    if (lpaplLongest NE NULL)
       *lpaplLongest = aplLongest;
    if (lpSymGlb NE NULL)
       *lpSymGlb     = hGlbSub;
    if (lpImmType NE NULL)
       *lpImmType    = immType;
    if (lpArrType NE NULL)
       *lpArrType    = TranslateImmTypeToArrayType (immType);;

    // If the return value is immediate, ...
    if (hGlbSub EQ NULL)
    {
        if (lpaplInteger NE NULL)
           *lpaplInteger = (APLINT) aplLongest;
        if (lpaplFloat NE NULL)
           *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
        if (lpaplChar NE NULL)
           *lpaplChar    = (APLCHAR) aplLongest;
    } // End IF
} // End GetNextValueToken


//***************************************************************************
//  $GetFirstValueToken
//
//  Return the first value from a token as either
//    both an integer and a float, or as a character,
//    or as an LPSYMENTRY/HGLOBAL.  The token may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void GetFirstValueToken
    (LPTOKEN      lpToken,      // Ptr to the token
     LPAPLINT     lpaplInteger, // Ptr to the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type:  ARRAY_TYPES (may be NULL)

{
    HGLOBAL hGlbData;

    // Fill in default values
    if (lpSymGlb NE NULL)
        *lpSymGlb = NULL;

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (hGlbData));

                break;      // Continue below with global case
            } // End IF

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                              lpToken->tkData.tkSym->stData.stLongest,
                              lpaplInteger,
                              lpaplFloat,
                              lpaplChar,
                              lpaplLongest,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_VARIMMED:
            // Handle the immediate case
            GetFirstValueImm (lpToken->tkFlags.ImmType,
                              lpToken->tkData.tkLongest,
                              lpaplInteger,
                              lpaplFloat,
                              lpaplChar,
                              lpaplLongest,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetFirstValueGlb (hGlbData,         // The global memory handle
                      lpaplInteger,     // Ptr to integer (or Boolean) (may be NULL)
                      lpaplFloat,       // ...    float (may be NULL)
                      lpaplChar,        // ...    char (may be NULL)
                      lpaplLongest,     // ...    longest (may be NULL)
                      lpSymGlb,         // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                      lpImmType,        // ...    immediate type (see IMM_TYPES) (may be NULL)
                      lpArrType,        // ...    array type -- ARRAY_TYPES (may be NULL)
                      TRUE);            // TRUE iff we should expand LPSYMENTRY into immediate value
} // End GetFirstValueToken


//***************************************************************************
//  $GetFirstValueImm
//
//  Return the first value of an immediate
//***************************************************************************

void GetFirstValueImm
    (IMM_TYPES    immType,      // The token's immediate type (see IMM_TYPES)
     APLLONGEST   aplLongest,   // The longest immediate value
     LPAPLINT     lpaplInteger, // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type:  ARRAY_TYPES (may be NULL)
{
    if (lpImmType NE NULL)
        *lpImmType    = immType;
    if (lpArrType NE NULL)
        *lpArrType    = TranslateImmTypeToArrayType (immType);
    if (lpaplLongest NE NULL)
        *lpaplLongest = aplLongest;

    // Split cases based upon the immediate type
    switch (immType)
    {
        case IMMTYPE_BOOL:
            if (lpaplInteger NE NULL)
                *lpaplInteger = (APLBOOL) aplLongest;
            if (lpaplFloat NE NULL)
                *lpaplFloat   = (APLFLOAT) (APLBOOL) aplLongest;
            if (lpaplChar NE NULL)
                *lpaplChar    = WC_EOS;
            break;

        case IMMTYPE_INT:
            if (lpaplInteger NE NULL)
                *lpaplInteger = (APLINT) aplLongest;
            if (lpaplFloat NE NULL)
                *lpaplFloat   = (APLFLOAT) (APLINT) aplLongest;
            if (lpaplChar NE NULL)
                *lpaplChar    = WC_EOS;
            break;

        case IMMTYPE_FLOAT:
            if (lpaplFloat NE NULL)
                *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
            if (lpaplInteger NE NULL)
                *lpaplInteger = (APLINT) *(LPAPLFLOAT) &aplLongest;
            if (lpaplChar NE NULL)
                *lpaplChar    = WC_EOS;
            break;

        case IMMTYPE_CHAR:
            if (lpaplInteger NE NULL)
                *lpaplInteger = 0;
            if (lpaplFloat NE NULL)
                *lpaplFloat   = 0;
            if (lpaplChar NE NULL)
                *lpaplChar    = (APLCHAR) aplLongest;
            break;

        defstop
            break;
    } // End SWITCH
} // End GetFirstValueImm


//***************************************************************************
//  $GetFirstValueGlb
//
//  Return the first value from an HGLOBAL as either
//    both an integer and a float, or as a character,
//    or as an HGLOBAL.  The HGLOBAL may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void GetFirstValueGlb
    (HGLOBAL      hGlbData,         // The global memory handle
     LPAPLINT     lpaplInteger,     // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,       // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,        // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest,     // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,         // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,        // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPAPLSTYPE   lpArrType,        // ...        array type -- ARRAY_TYPES (may be NULL)
     UBOOL        bExpandSym)       // TRUE iff we should expand LPSYMENTRY into immediate value

{
    LPVOID     lpMem;               // Ptr to global memory
    APLSTYPE   aplType;             // Storage type
    APLNELM    aplNELM;             // NELM
    APLINT     aplInteger;          // Temp integer
    APLFLOAT   aplFloat;            // ...  float
    APLCHAR    aplChar;             // ...  char
    LPSYMENTRY lpSym;               // Ptr to SYMENTRY in HETERO or NESTED cases

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Split cases based upon the signature
    switch (GetSignatureMem (lpMem))
    {
        case LSTARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPLSTARRAY_HEADER) lpMem)
            // Get the Array Type and NELM
            aplType = ARRAY_LIST;
            aplNELM = lpHeader->NELM;

            // Skip over the header to the data
            lpMem = LstArrayBaseToData (lpMem);
#undef  lpHeader
            break;

        case VARARRAY_HEADER_SIGNATURE:
#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
            // Get the Array Type and NELM
            aplType = lpHeader->ArrType;
            aplNELM = lpHeader->NELM;

            // Skip over the header and dimensions to the data
            lpMem = VarArrayDataFmBase (lpMem);
#undef  lpHeader
            break;

        case DFN_HEADER_SIGNATURE:
        case FCNARRAY_HEADER_SIGNATURE:
        defstop
    } // End SWITCH

    if (lpImmType NE NULL)
        *lpImmType = TranslateArrayTypeToImmType (aplType);
    if (lpArrType NE NULL)
        *lpArrType = aplType;

    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            // If the array is empty and numeric, the value must be Boolean or APA
            aplInteger = aplNELM ? (BIT0 & *(LPAPLBOOL) lpMem) : 0;

            if (lpaplInteger NE NULL)
                *lpaplInteger = aplInteger;
            if (lpaplFloat NE NULL)
                *lpaplFloat   = (APLFLOAT) aplInteger;
            if (lpaplChar NE NULL)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest NE NULL)
                *lpaplLongest = aplInteger;
            if (lpSymGlb NE NULL)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_INT:
            aplInteger = aplNELM ? *(LPAPLINT) lpMem : 0;

            if (lpaplInteger NE NULL)
                *lpaplInteger = aplInteger;
            if (lpaplFloat NE NULL)
                *lpaplFloat   = (APLFLOAT) aplInteger;
            if (lpaplChar NE NULL)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest NE NULL)
                *lpaplLongest = aplInteger;
            if (lpSymGlb NE NULL)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMem)
            // If the array is empty and numeric, the value must be Boolean or APA
            aplInteger = aplNELM ? lpAPA->Off : 0;
#undef  lpAPA
            if (lpaplInteger NE NULL)
                *lpaplInteger = aplInteger;
            if (lpaplFloat NE NULL)
                *lpaplFloat   = (APLFLOAT) aplInteger;
            if (lpaplChar NE NULL)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest NE NULL)
                *lpaplLongest = aplInteger;
            if (lpSymGlb NE NULL)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_FLOAT:
            aplFloat = aplNELM ? *(LPAPLFLOAT) lpMem : 0;

            if (lpaplFloat NE NULL)
                *lpaplFloat   = aplFloat;
            if (lpaplInteger NE NULL)
                *lpaplInteger = (APLINT) aplFloat;
            if (lpaplChar NE NULL)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest NE NULL)
                *lpaplLongest = *(LPAPLLONGEST) &aplFloat;
            if (lpSymGlb NE NULL)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_CHAR:
            // If the array is empty and char, the value must be blank
            aplChar = aplNELM ? (*(LPAPLCHAR) lpMem) : L' ';

            if (lpaplInteger NE NULL)
                *lpaplInteger = 0;
            if (lpaplFloat NE NULL)
                *lpaplFloat   = 0;
            if (lpaplChar NE NULL)
                *lpaplChar    = aplChar;
            if (lpaplLongest NE NULL)
                *lpaplLongest = aplChar;
            if (lpSymGlb NE NULL)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_LIST:
#define lptkList        ((LPAPLLIST) lpMem)
            // Split cases based upon the token type
            switch (lptkList->tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Fill in default values
                    if (lpSymGlb NE NULL)
                        *lpSymGlb = NULL;

                    // Handle the immediate case
                    GetFirstValueImm (lptkList->tkFlags.ImmType,
                                      lptkList->tkData.tkLongest,
                                      lpaplInteger,
                                      lpaplFloat,
                                      lpaplChar,
                                      lpaplLongest,
                                      lpImmType,
                                      lpArrType);
                    break;

                case TKT_VARARRAY:
                    // Handle the global case
                    GetFirstValueGlb (lptkList->tkData.tkGlbData,   // The global memory handle
                                      lpaplInteger,                 // Ptr to integer (or Boolean) (may be NULL)
                                      lpaplFloat,                   // ...    float (may be NULL)
                                      lpaplChar,                    // ...    char (may be NULL)
                                      lpaplLongest,                 // ...    longest (may be NULL)
                                      lpSymGlb,                     // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                                      lpImmType,                    // ...    immediate type (see IMM_TYPES) (may be NULL)
                                      lpArrType,                    // ...    array type -- ARRAY_TYPES (may be NULL)
                                      FALSE);                       // TRUE iff we should expand LPSYMENTRY into immediate value
                    break;

                case TKT_LISTSEP:
                    if (lpaplInteger NE NULL)
                        *lpaplInteger = 0;
                    if (lpaplFloat NE NULL)
                        *lpaplFloat   = 0;
                    if (lpaplChar NE NULL)
                        *lpaplChar    = WC_EOS;
                    if (lpaplLongest NE NULL)
                        *lpaplLongest = WC_EOS;
                    if (lpSymGlb NE NULL)
                        *lpSymGlb     = NULL;
                    break;

                defstop
                    break;
            } // End SWITCH
#undef  lptkList
            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            lpSym = *(LPAPLHETERO) lpMem;

            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (lpSym))
            {
                case PTRTYPE_STCONST:
                    // stData is immediate
                    Assert (lpSym->stFlags.Imm);

                    if (!bExpandSym
                     && lpSymGlb)
                    {
                        *lpSymGlb = lpSym;

                        break;
                    } // End IF

                    Assert (IsNested (aplType) || !IsEmpty (aplNELM));

                    // Fill in default values
                    if (lpSymGlb NE NULL)
                        *lpSymGlb = NULL;

                    // Handle the immediate case
                    GetFirstValueImm (lpSym->stFlags.ImmType,
                                      lpSym->stData.stLongest,
                                      lpaplInteger,
                                      lpaplFloat,
                                      lpaplChar,
                                      lpaplLongest,
                                      lpImmType,
                                      lpArrType);
                    break;

                case PTRTYPE_HGLOBAL:
                    if (lpaplInteger NE NULL)
                        *lpaplInteger = 0;
                    if (lpaplFloat NE NULL)
                        *lpaplFloat   = 0;
                    if (lpaplChar NE NULL)
                        *lpaplChar    = WC_EOS;
                    if (lpaplLongest NE NULL)
                        *lpaplLongest = 0;
                    if (lpSymGlb NE NULL)
                        *lpSymGlb     = *(LPAPLNESTED) lpMem;
                    if (lpImmType NE NULL)
                    {
                        AttrsOfGlb (*(LPAPLNESTED) lpMem, &aplType, NULL, NULL, NULL);
                        *lpImmType    = TranslateArrayTypeToImmType (aplType);
                    } // End IF

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:
        case ARRAY_VFP:
        case ARRAY_ARB:
        case ARRAY_HC2I:
        case ARRAY_HC2F:
        case ARRAY_HC2R:
        case ARRAY_HC2V:
        case ARRAY_HC4I:
        case ARRAY_HC4F:
        case ARRAY_HC4R:
        case ARRAY_HC4V:
        case ARRAY_HC8I:
        case ARRAY_HC8F:
        case ARRAY_HC8R:
        case ARRAY_HC8V:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            if (lpaplInteger NE NULL)
                *lpaplInteger = 0;
            if (lpaplFloat NE NULL)
                *lpaplFloat   = 0;
            if (lpaplChar NE NULL)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest NE NULL)
                *lpaplLongest = 0;
            // If the array is empty, ...
            if (IsEmpty (aplNELM))
            {
                if (lpSymGlb NE NULL)
                // Split cases based upon the storage type
                switch (aplType)
                {
                    case ARRAY_RAT:
                        *lpSymGlb = &mpqZero;

                        break;

                    case ARRAY_VFP:
                        *lpSymGlb = &mpfZero;

                        break;

                    case ARRAY_ARB:
                        *lpSymGlb = &arbZero;

                        break;

                    defstop
                        break;
                } // End IF/SWITCH
            } else
                if (lpSymGlb NE NULL)
                    *lpSymGlb  = lpMem;
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End GetFirstValueGlb


//***************************************************************************
//  $GetNextValueMemIntoToken
//
//  Get the next value from global memory into a token
//  Note that the nested global case increments the reference count
//***************************************************************************

UBOOL GetNextValueMemIntoToken
    (APLUINT  uArg,                         // Index to use
     LPVOID   lpMemArg,                     // Ptr to global memory object to index
     APLSTYPE aplTypeArg,                   // Storage type of the arg
     APLNELM  aplNELMArg,                   // NELM         ...
     APLINT   apaOff,                       // APA offset (if needed)
     APLINT   apaMul,                       // APA multiplier (if needed)
     LPTOKEN  lptkArg)                      // Ptr to token in which to place the value

{
    // Clear the NoDisplay flag
    lptkArg->tkFlags.NoDisplay = FALSE;

    // If the arg is empty,
    //   and not a ptr array, ...
    if (IsEmpty (aplNELMArg)
     && !IsPtrArray (aplTypeArg))
    {
        APLRAT aplRat = {0};
        APLVFP aplVfp = {0};
        APLARB aplArb = {0};

        Assert (uArg EQ 0);

        // Split cases based upon the arg storage type
        switch (aplTypeArg)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
                lptkArg->tkFlags.TknType  = TKT_VARIMMED;
                lptkArg->tkFlags.ImmType  = IMMTYPE_BOOL;
                lptkArg->tkData.tkInteger = 0;

                break;

            case ARRAY_CHAR:
                lptkArg->tkFlags.TknType  = TKT_VARIMMED;
                lptkArg->tkFlags.ImmType  = IMMTYPE_CHAR;
                lptkArg->tkData.tkChar    = L' ';

                break;

            case ARRAY_RAT:
                // Initialize to 0/1
                mpq_init (&aplRat);

                lptkArg->tkFlags.TknType  = TKT_VARARRAY;
                lptkArg->tkFlags.ImmType  = IMMTYPE_RAT;
                lptkArg->tkData.tkGlbData =
                  MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                                  &aplRat,                          // Ptr to the value
                                   FALSE,                           // TRUE iff we should initialize the target first
                                   lptkArg);                        // Ptr to function token
                if (lptkArg->tkData.tkGlbData EQ NULL)
                    goto ERROR_EXIT;
                break;

            case ARRAY_VFP:
                // Initialize to 0
                mpfr_init0 (&aplVfp);

                lptkArg->tkFlags.TknType  = TKT_VARARRAY;
                lptkArg->tkFlags.ImmType  = IMMTYPE_VFP;
                lptkArg->tkData.tkGlbData =
                  MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                                  &aplVfp,                          // Ptr to the value
                                   FALSE,                           // TRUE iff we should initialize the target first
                                   lptkArg);                        // Ptr to function token
                if (lptkArg->tkData.tkGlbData EQ NULL)
                    goto ERROR_EXIT;
                break;

            case ARRAY_ARB:
                // Initialize to 0/0
                Myarb_init (&aplArb);

                lptkArg->tkFlags.TknType  = TKT_VARARRAY;
                lptkArg->tkFlags.ImmType  = IMMTYPE_ARB;
                lptkArg->tkData.tkGlbData =
                  MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                                  &aplArb,                          // Ptr to the value
                                   FALSE,                           // TRUE iff we should initialize the target first
                                   lptkArg);                        // Ptr to function token
                if (lptkArg->tkData.tkGlbData EQ NULL)
                    goto ERROR_EXIT;
                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
            defstop
                break;
        } // End SWITCH
    } else
    // Split cases based upon the arg storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_BOOL;
            lptkArg->tkData.tkInteger = BIT0 & (UCHAR) (((LPAPLBOOL) lpMemArg)[uArg >> LOG2NBIB] >> (MASKLOG2NBIB & uArg));

            break;

        case ARRAY_INT:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
            lptkArg->tkData.tkInteger = ((LPAPLINT) lpMemArg)[uArg];

            break;

        case ARRAY_FLOAT:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_FLOAT;
            lptkArg->tkData.tkFloat   = ((LPAPLFLOAT) lpMemArg)[uArg];

            break;

        case ARRAY_CHAR:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_CHAR;
            lptkArg->tkData.tkChar    = ((LPAPLCHAR) lpMemArg)[uArg];

            break;

        case ARRAY_APA:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_INT;
            lptkArg->tkData.tkInteger = (apaOff + apaMul * uArg);

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (((LPAPLNESTED) lpMemArg)[uArg]))
            {
                case PTRTYPE_STCONST:
                    lptkArg->tkFlags.TknType  = TKT_VARIMMED;
                    lptkArg->tkFlags.ImmType  = ((LPAPLHETERO) lpMemArg)[uArg]->stFlags.ImmType;
                    lptkArg->tkData.tkLongest = ((LPAPLHETERO) lpMemArg)[uArg]->stData.stLongest;

                    break;

                case PTRTYPE_HGLOBAL:
                    lptkArg->tkFlags.TknType  = TKT_VARARRAY;
                    lptkArg->tkFlags.ImmType  = IMMTYPE_ERROR;
                    lptkArg->tkData.tkGlbData = CopySymGlbDir_PTB (((LPAPLNESTED) lpMemArg)[uArg]);

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_RAT;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLRAT) lpMemArg)[uArg],     // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_VFP:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_VFP;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLVFP) lpMemArg)[uArg],     // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_ARB:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_ARB;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLARB) lpMemArg)[uArg],     // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC2I:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC2I;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC2I) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC4I:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC4I;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC4I) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC8I:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC8I;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC8I) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC2F:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC2F;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC2F) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC4F:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC4F;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC4F) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC8F:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC8F;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC8F) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC2R:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC2R;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC2R) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC4R:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC4R;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC4R) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC8R:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC8R;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC8R) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC2V:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC2V;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC2V) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC4V:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC4V;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC4V) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_HC8V:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_HC8V;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLHC8V) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_BA2F:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_BA2F;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLBA2F) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_BA4F:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_BA4F;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLBA4F) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        case ARRAY_BA8F:
            lptkArg->tkFlags.TknType  = TKT_VARARRAY;
            lptkArg->tkFlags.ImmType  = IMMTYPE_BA8F;
            lptkArg->tkData.tkGlbData =
              MakeGlbEntry_EM (aplTypeArg,                      // Entry type
                              &((LPAPLBA8F) lpMemArg)[uArg],    // Ptr to the value
                               TRUE,                            // TRUE iff we should initialize the target first
                               lptkArg);                        // Ptr to function token
            if (lptkArg->tkData.tkGlbData EQ NULL)
                goto ERROR_EXIT;
            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    return TRUE;

ERROR_EXIT:
    // Set the token type to something FreeResultTkn will ignore
    lptkArg->tkFlags.TknType  = TKT_VARIMMED;

    return FALSE;
} // End GetNextValueMemIntoToken


//***************************************************************************
//  $GetNextRatIntGlb
//
//  Get the next value from a rational array global memory handle
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextRatIntGlb
    (HGLOBAL hGlbRat,                       // Global memory handle
     APLINT  uRes,                          // Index
     LPUBOOL lpbRet)                        // Ptr to TRUE iff the result is valid

{
    LPAPLRAT lpMemRat;
    APLINT   aplInteger;

    Assert (IsGlbTypeVarDir_PTB (hGlbRat));

    // Lock the memory to get a ptr to it
    lpMemRat = MyGlobalLockVar (hGlbRat);

    // Skip over the header and dimensions to the data
    lpMemRat = VarArrayDataFmBase (lpMemRat);

    // Convert the next RAT to an INT using System []CT
    aplInteger = mpq_get_sctsx (&lpMemRat[uRes], lpbRet);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRat); lpMemRat = NULL;

    return aplInteger;
} // End GetNextRatIntGlb


//***************************************************************************
//  $GetNextRatIntMem
//
//  Get the next value from a rational array global memory handle
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextRatIntMem
    (LPAPLRAT lpMemRat,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    // Convert the next RAT to an INT using System []CT
    return mpq_get_sctsx (&lpMemRat[uRes], lpbRet);
} // End GetNextRatIntMem


//***************************************************************************
//  $GetNextRatFltMem
//
//  Get the next value from a rational array global memory ptr
//***************************************************************************

APLFLOAT GetNextRatFltMem
    (LPAPLRAT lpMemRat,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    APLVFP mpfTmp = {0};

    // Initialize the temp
    mpfr_init (&mpfTmp);

    // Copy the RAT to a VFP
    mpfr_set_q (&mpfTmp, &lpMemRat[uRes], MPFR_RNDN);

    // Zero the sign
    mpfr_abs (&mpfTmp, &mpfTmp, MPFR_RNDN);

    // Compare against the maximum float
    if (mpfr_cmp_d (&mpfTmp, DBL_MAX) > 0
     || (mpfr_cmp_d (&mpfTmp, DBL_MIN) < 0
      && !mpfr_zero_p (&mpfTmp)))
        *lpbRet = FALSE;
    else
        *lpbRet = TRUE;
    // We no longer need this storage
    Myf_clear (&mpfTmp);

    // If the value is in range, ...
    if (*lpbRet)
        // Convert the next RAT to an FLOAT
        return mpq_get_d (&lpMemRat[uRes]);
    else
        return 0.0;
} // End GetNextRatFltMem


//***************************************************************************
//  $GetNextVfpIntGlb
//
//  Get the next value from a VFP array global memory handle
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextVfpIntGlb
    (HGLOBAL hGlbVfp,                       // Global memory handle
     APLINT  uRes,                          // Index
     LPUBOOL lpbRet)                        // Ptr to TRUE iff the result is valid

{
    LPAPLVFP lpMemVfp;
    APLINT   aplInteger;

    Assert (IsGlbTypeVarDir_PTB (hGlbVfp));

    // Lock the memory to get a ptr to it
    lpMemVfp = MyGlobalLockVar (hGlbVfp);

    // Skip over the header and dimensions to the data
    lpMemVfp = VarArrayDataFmBase (lpMemVfp);

    // Convert the next VFP to an INT using System []CT
    aplInteger = mpfr_get_sctsx (&lpMemVfp[uRes], lpbRet);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbVfp); lpMemVfp = NULL;

    return aplInteger;
} // End GetNextVfpIntGlb


//***************************************************************************
//  $GetNextVfpIntMem
//
//  Get the next value from a VFP array global memory ptr
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextVfpIntMem
    (LPAPLVFP lpMemVfp,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    // Convert the next VFP to an INT using System []CT
    return mpfr_get_sctsx (&lpMemVfp[uRes], lpbRet);
} // End GetNextVfpIntMem


//***************************************************************************
//  $GetNextVfpFltMem
//
//  Get the next value from a VFP array global memory ptr
//***************************************************************************

APLFLOAT GetNextVfpFltMem
    (LPAPLVFP lpMemVfp,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    APLVFP mpfTmp = {0};

    // Initialize the temp
    mpfr_init (&mpfTmp);

    // Copy the VFP
    mpfr_set (&mpfTmp, &lpMemVfp[uRes], MPFR_RNDN);

    // Zero the sign
    mpfr_abs (&mpfTmp, &mpfTmp, MPFR_RNDN);

    // Compare against the maximum float
    if (mpfr_cmp_d (&mpfTmp, DBL_MAX) > 0
     || (mpfr_cmp_d (&mpfTmp, DBL_MIN) < 0
      && !mpfr_zero_p (&mpfTmp)))
        *lpbRet = FALSE;
    else
        *lpbRet = TRUE;
    // We no longer need this storage
    Myf_clear (&mpfTmp);

    // If the value is in range, ...
    if (*lpbRet)
        // Convert the next VFP to an FLOAT
        return mpfr_get_d (&lpMemVfp[uRes], MPFR_RNDN);
    else
        return 0.0;
} // End GetNextVfpFltMem


//***************************************************************************
//  $GetNextArbIntGlb
//
//  Get the next value from a ARB array global memory handle
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextArbIntGlb
    (HGLOBAL hGlbArb,                       // Global memory handle
     APLINT  uRes,                          // Index
     LPUBOOL lpbRet)                        // Ptr to TRUE iff the result is valid

{
    LPAPLARB lpMemArb;
    APLINT   aplInteger;

    Assert (IsGlbTypeVarDir_PTB (hGlbArb));

    // Lock the memory to get a ptr to it
    lpMemArb = MyGlobalLockVar (hGlbArb);

    // Skip over the header and dimensions to the data
    lpMemArb = VarArrayDataFmBase (lpMemArb);

    // Convert the next ARB to an INT using System []CT
    aplInteger = arb_get_sctsx (&lpMemArb[uRes], lpbRet);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArb); lpMemArb = NULL;

    return aplInteger;
} // End GetNextArbIntGlb


//***************************************************************************
//  $GetNextArbIntMem
//
//  Get the next value from a ARB array global memory ptr
//    as an integer using System []CT
//***************************************************************************

APLINT GetNextArbIntMem
    (LPAPLARB lpMemArb,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    // Convert the next ARB to an INT using System []CT
    return arb_get_sctsx (&lpMemArb[uRes], lpbRet);
} // End GetNextArbIntMem


//***************************************************************************
//  $GetNextArbFltMem
//
//  Get the next value from a ARB array global memory ptr
//***************************************************************************

APLFLOAT GetNextArbFltMem
    (LPAPLARB lpMemArb,                     // Ptr to global memory
     APLINT   uRes,                         // Index
     LPUBOOL  lpbRet)                       // Ptr to TRUE iff the result is valid

{
    APLARB arbTmp = {0};

    // Initialize the temp
    Myarb_init (&arbTmp);

    // Copy the RAT to a ARB
    arb_set (&arbTmp, &lpMemArb[uRes]);

    // Zero the sign
    arb_abs2 (&arbTmp, &arbTmp);

    // Compare against the maximum float
    if (arb_cmp_d (&arbTmp, DBL_MAX) > 0
     || (arb_cmp_d (&arbTmp, DBL_MIN) < 0
      && !arb_zero_p (&arbTmp)))
        *lpbRet = FALSE;
    else
        *lpbRet = TRUE;
    // We no longer need this storage
    Myarb_clear (&arbTmp);

    // If the value is in range, ...
    if (*lpbRet)
        // Convert the next ARB to an FLOAT
        return arb_get_d (&lpMemArb[uRes]);
    else
        return 0.0;
} // End GetNextArbFltMem


//***************************************************************************
//  $GetNextQword
//
//  Return the next value as a QWORD
//***************************************************************************

APLINT GetNextQword
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return BIT0 & (((LPAPLBOOL) lpMem)[uRes >> LOG2NBIB] >> (MASKLOG2NBIB & uRes));

        case ARRAY_INT:
        case ARRAY_FLOAT:
            return ((LPAPLINT) lpMem)[uRes];

        case ARRAY_APA:
            return ((LPAPLAPA) lpMem)->Off + ((LPAPLAPA) lpMem)->Mul * uRes;

        case ARRAY_HC2I:
        case ARRAY_HC2F:
            return ((LPAPLHC2I) lpMem)[uRes].parts[0];

        case ARRAY_HC4I:
        case ARRAY_HC4F:
            return ((LPAPLHC4I) lpMem)[uRes].parts[0];

        case ARRAY_HC8I:
        case ARRAY_HC8F:
            return ((LPAPLHC8I) lpMem)[uRes].parts[0];

        defstop
            return 0;
    } // End SWITCH
} // End GetNextQword


//***************************************************************************
//  $GetNextInteger
//
//  Return the next value as an integer
//***************************************************************************

APLINT GetNextInteger
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return BIT0 & (((LPAPLBOOL) lpMem)[uRes >> LOG2NBIB] >> (MASKLOG2NBIB & uRes));

        case ARRAY_INT:
            return ((LPAPLINT) lpMem)[uRes];

        case ARRAY_FLOAT:
            return (APLINT) ((LPAPLFLOAT) lpMem)[uRes];

        case ARRAY_APA:
            return ((LPAPLAPA) lpMem)->Off + ((LPAPLAPA) lpMem)->Mul * uRes;
#if FALSE   // Keep around in case we actually need it
        case ARRAY_HC2I:
            return *(LPAPLINT) &((LPAPLHC2I) lpMem)[uRes];

        case ARRAY_HC4I:
            return *(LPAPLINT) &((LPAPLHC4I) lpMem)[uRes];

        case ARRAY_HC8I:
            return *(LPAPLINT) &((LPAPLHC8I) lpMem)[uRes];

        case ARRAY_HC2F:
            return (APLINT) ((LPAPLHC2F) lpMem)[uRes].parts[0];

        case ARRAY_HC4F:
            return (APLINT) ((LPAPLHC4F) lpMem)[uRes].parts[0];

        case ARRAY_HC8F:
            return (APLINT) ((LPAPLHC8F) lpMem)[uRes].parts[0];
#endif
        defstop
            return 0;
    } // End SWITCH
} // End GetNextInteger


//***************************************************************************
//  $GetNextFloat
//
//  Return the next value as a float
//***************************************************************************

APLFLOAT GetNextFloat
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return (APLFLOAT) (BIT0 & (((LPAPLBOOL) lpMem)[uRes >> LOG2NBIB] >> (MASKLOG2NBIB & uRes)));

        case ARRAY_INT:
            return (APLFLOAT) ((LPAPLINT) lpMem)[uRes];

        case ARRAY_FLOAT:
            return ((LPAPLFLOAT) lpMem)[uRes];

        case ARRAY_APA:
            return (APLFLOAT) (((LPAPLAPA) lpMem)->Off + ((LPAPLAPA) lpMem)->Mul * uRes);
#if FALSE   // Keep around in case we actually need it
        case ARRAY_HC2I:
            return (APLFLOAT) ((LPAPLHC2I) lpMem)[uRes].parts[0];

        case ARRAY_HC4I:
            return (APLFLOAT) ((LPAPLHC4I) lpMem)[uRes].parts[0];

        case ARRAY_HC8I:
            return (APLFLOAT) ((LPAPLHC8I) lpMem)[uRes].parts[0];

        case ARRAY_HC2F:
            return *(LPAPLFLOAT) &((LPAPLHC2F) lpMem)[uRes];

        case ARRAY_HC4F:
            return *(LPAPLFLOAT) &((LPAPLHC4F) lpMem)[uRes];

        case ARRAY_HC8F:
            return *(LPAPLFLOAT) &((LPAPLHC8F) lpMem)[uRes];
#endif
        defstop
            return 0;
    } // End SWITCH
} // End GetNextFloat


//***************************************************************************
//  $GetNextRat_INIT
//
//  Return the next value as an initialized  RAT
//***************************************************************************

APLRAT GetNextRat_INIT
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    APLRAT aplRes = {0};

    // Initialize to 0/1
    mpq_init (&aplRes);

    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Get the next value
            mpq_set_sx (&aplRes, GetNextInteger (lpMem, aplType, uRes), 1);

            break;

        case ARRAY_FLOAT:
            // Get the next value
            mpq_set_d  (&aplRes, GetNextFloat   (lpMem, aplType, uRes));

            break;

        case ARRAY_RAT:
            // Get the next value
            mpq_set    (&aplRes,    &((LPAPLRAT) lpMem)[uRes]         );

            break;

        case ARRAY_VFP:
            // Get the next value
            mpq_set_fr (&aplRes,    &((LPAPLVFP) lpMem)[uRes]         );

            break;

        case ARRAY_ARB:
        defstop
            break;
    } // End SWITCH

    return aplRes;
} // End GetNextRat_INIT


//***************************************************************************
//  $GetNextRatPtr_XFREE
//
//  Return the next value as a RAT w/o having to free it
//***************************************************************************

LPAPLRAT GetNextRatPtr_XFREE
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_RAT:
            // Return the next value
            return &((LPAPLRAT) lpMem)[uRes];

        case ARRAY_HC2R:
            // Return the next value
            return (LPAPLRAT) &((LPAPLHC2R) lpMem)[uRes];

        case ARRAY_HC4R:
            // Return the next value
            return (LPAPLRAT) &((LPAPLHC4R) lpMem)[uRes];

        case ARRAY_HC8R:
            // Return the next value
            return (LPAPLRAT) &((LPAPLHC8R) lpMem)[uRes];

        defstop
            return NULL;
    } // End SWITCH
} // End GetNextRatPtr_XFREE


//***************************************************************************
//  $GetNextVfp_INIT
//
//  Return the next value as an initialized VFP
//***************************************************************************

APLVFP GetNextVfp_INIT
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    APLVFP aplRes = {0};

    // Initialize to 0
    mpfr_init0 (&aplRes);

    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Get the next value
            mpfr_set_sx (&aplRes, GetNextInteger (lpMem, aplType, uRes), MPFR_RNDN);

            break;

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
        case ARRAY_FLOAT:
            // Get the next value
            mpfr_set_d  (&aplRes, GetNextFloat   (lpMem, aplType, uRes), MPFR_RNDN);

            break;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
        case ARRAY_RAT:
            // Get the next value
            mpfr_set_q  (&aplRes, GetNextRatPtr_XFREE (lpMem, aplType, uRes), MPFR_RNDN);

            break;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
        case ARRAY_VFP:
            // Get the next value
            mpfr_set    (&aplRes, GetNextVfpPtr_XFREE (lpMem, aplType, uRes), MPFR_RNDN);

            break;

        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
        case ARRAY_ARB:
        defstop
            break;
    } // End SWITCH

    return aplRes;
} // End GetNextVfp_INIT


//***************************************************************************
//  $GetNextVfpPtr_XFREE
//
//  Return the next value as a VFP w/o having to free it
//***************************************************************************

LPAPLVFP GetNextVfpPtr_XFREE
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_VFP:
            // Return the next value
            return &((LPAPLVFP) lpMem)[uRes];

        case ARRAY_HC2V:
            // Return the next value
            return (LPAPLVFP) &((LPAPLHC2V) lpMem)[uRes];

        case ARRAY_HC4V:
            // Return the next value
            return (LPAPLVFP) &((LPAPLHC4V) lpMem)[uRes];

        case ARRAY_HC8V:
            // Return the next value
            return (LPAPLVFP) &((LPAPLHC8V) lpMem)[uRes];

        defstop
            return NULL;
    } // End SWITCH
} // End GetNextVfpPtr_XFREE


//***************************************************************************
//  $GetNextArb_INIT
//
//  Return the next value as an initialized ARB
//***************************************************************************

APLBA8F GetNextArb_INIT
    (LPVOID   lpMem,            // Ptr to global memory
     APLSTYPE aplType,          // Storage type
     APLINT   uRes,             // Index
     int      iHCDim)           // HC Dimension (1, 2, 4, 8)

{
    APLBA8F aplRes = {0};           // The result
    int     i;                      // Loop counter
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize to 0/0
    arbX_init (&aplRes, iHCDim);

    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Get the next value
            arb_set_sx   (&aplRes.parts[0], GetNextInteger (lpMem, aplType, uRes));

            break;

        case ARRAY_FLOAT:
            // Get the next value
            arb_set_d    (&aplRes.parts[0], GetNextFloat   (lpMem, aplType, uRes));

            break;

        case ARRAY_RAT:
            // Get the next value
            arb_set_q    (&aplRes.parts[0],    &((LPAPLRAT) lpMem)[uRes]         );

            break;

        case ARRAY_VFP:
            // Get the next value
            arb_set_mpfr (&aplRes.parts[0],    &((LPAPLVFP) lpMem)[uRes]         );

            break;

        case ARRAY_BA1F:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Loop through the parts
            for (i = 0; i < iHCDim; i++)
                // Get the next value
                arb_set      (&aplRes.parts[i],    &((LPAPLBA8F) lpMem)[uRes].parts[i]);
            break;

        defstop
            break;
    } // End SWITCH

    return aplRes;
} // End GetNextArb_INIT


//***************************************************************************
//  $GetNextArbPtr_XFREE
//
//  Return the next value as a ARB w/o having to free it
//***************************************************************************

LPAPLARB GetNextArbPtr_XFREE
    (LPVOID   lpMem,                        // Ptr to global memory
     APLSTYPE aplType,                      // Storage type
     APLINT   uRes)                         // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_ARB:
            // Return the next value
            return &((LPAPLARB) lpMem)[uRes];

        case ARRAY_BA2F:
            // Return the next value
            return (LPAPLARB) &((LPAPLBA2F) lpMem)[uRes];

        case ARRAY_BA4F:
            // Return the next value
            return (LPAPLARB) &((LPAPLHC4F) lpMem)[uRes];

        case ARRAY_BA8F:
            // Return the next value
            return (LPAPLARB) &((LPAPLHC8F) lpMem)[uRes];

        defstop
            return NULL;
    } // End SWITCH
} // End GetNextArbPtr_XFREE


//***************************************************************************
//  $GetNextHetero
//
//  Return the next value as Boolean/Integer, Float, Char, Rat, Vfp, or Arb
//***************************************************************************

APLSTYPE GetNextHetero
    (LPAPLHETERO lpMem,                     // Ptr to global memory
     APLINT      uRes,                      // Index
     LPAPLINT    lpaplInteger,              // Ptr to Boolean/Integer result (may be NULL)
     LPAPLFLOAT  lpaplFloat,                // Ptr to Float result           (may be NULL)
     LPAPLCHAR   lpaplChar,                 // Ptr to Char result            (may be NULL)
     HGLOBAL    *lplpSymGlb)                // Ptr to global memory handle   (may be NULL)

{
    LPSYMENTRY lpSymEntry;
    HGLOBAL    lpSymGlb;
    APLSTYPE   aplType;

    // Get the next LPSYMENTRY
    lpSymEntry = lpMem[uRes];

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpSymEntry))
    {
        case PTRTYPE_STCONST:
            // stData is immediate
            Assert (lpSymEntry->stFlags.Imm);

            // Split cases based upon the immediate storage type
            switch (lpSymEntry->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    if (lpaplInteger NE NULL)
                        *lpaplInteger = lpSymEntry->stData.stBoolean;
                    if (lpaplFloat   NE NULL)
                        *lpaplFloat   = (APLFLOAT) *lpaplInteger;

                    aplType = ARRAY_BOOL;

                    break;

                case IMMTYPE_INT:
                    if (lpaplInteger NE NULL)
                        *lpaplInteger = lpSymEntry->stData.stInteger;
                    if (lpaplFloat   NE NULL)
                        *lpaplFloat   = (APLFLOAT) *lpaplInteger;

                    aplType = ARRAY_INT;

                    break;

                case IMMTYPE_FLOAT:
                    if (lpaplFloat   NE NULL)
                        *lpaplFloat   = lpSymEntry->stData.stFloat;

                    aplType = ARRAY_FLOAT;

                    break;

                case IMMTYPE_CHAR:
                    if (lpaplChar    NE NULL)
                        *lpaplChar    = lpSymEntry->stData.stChar;

                    aplType = ARRAY_CHAR;

                    break;

                defstop
                    aplType = ARRAY_ERROR;

                    break;
            } // End SWITCH

           lpSymGlb = &lpSymEntry->stData.stLongest;

            break;

        case PTRTYPE_HGLOBAL:
            // Copy the LPSYMENTRY
            lpSymGlb = lpSymEntry;

            // Get the attributes (Type, NELM, and Rank) of the global
            AttrsOfGlb (lpSymGlb, &aplType, NULL, NULL, NULL);

            // If the arg is global numeric, ...
            if (IsGlbNum (aplType))
            {
                Assert (IsGlbTypeVarDir_PTB (lpSymGlb));

                // Clear the ptr type bits
                lpSymGlb = ClrPtrTypeDir (lpSymGlb);

                // Skip over the header to the data
                lpSymGlb = VarArrayDataFmBase (lpSymGlb);
            } // End IF

            break;

        defstop
            aplType = ARRAY_ERROR;

            break;
    } // End SWITCH

    if (lplpSymGlb NE NULL)
       *lplpSymGlb = lpSymGlb;

    return aplType;
} // End GetNextHetero


//***************************************************************************
//  $GetNextHetero2
//
//  Return the next value as Boolean/Integer, Float, Char, Rat, Vfp, or Arb
//  Ensure that if lplpSymGlb is valid, its value is set.
//***************************************************************************

APLSTYPE GetNextHetero2
    (LPAPLHETERO lpMem,                     // Ptr to global memory
     APLINT      uRes,                      // Index
     LPAPLINT    lpaplInteger,              // Ptr to Boolean/Integer result
     LPAPLFLOAT  lpaplFloat,                // Ptr to Float result
     LPAPLCHAR   lpaplChar,                 // Ptr to Char result
     HGLOBAL    *lplpSymGlb)                // Ptr to global memory handle (may be NULL)

{
    APLSTYPE aplTypeRes;

    // Call common subroutine
    aplTypeRes = GetNextHetero (lpMem, uRes, lpaplInteger, lpaplFloat, lpaplChar, lplpSymGlb);

    // If the ptr is valid but unset, ...
    if (lplpSymGlb NE NULL
     && *lplpSymGlb EQ NULL)
        // Set it to the data in the STE
        *lplpSymGlb = &lpMem[uRes]->stData.stLongest;

    return aplTypeRes;
} // End GetNextHetero2


//***************************************************************************
//  $GetNextItemGlb
//
//  Get next item from a global memory handle
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextItemGlb
    (HGLOBAL     hGlbSub,                   // Item global memory handle
     APLUINT     uSub,                      // Index into item
     HGLOBAL    *lphGlbRes,                 // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes)           // Ptr to result immediate value (may be NULL)

{
    APLSTYPE  aplTypeSub;                   // Item storage type
    APLNELM   aplNELMSub;                   // Item NELM
    APLRANK   aplRankSub;                   // Item rank
    LPVOID    lpMemSub;                     // Ptr to item global memory

    // Lock the memory to get a ptr to it
    lpMemSub = MyGlobalLockVar (hGlbSub);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemSub)
    // Get the Array Type and Rank
    aplTypeSub = lpHeader->ArrType;
    aplNELMSub = lpHeader->NELM;
    aplRankSub = lpHeader->Rank;
#undef  lpHeader

    Assert (uSub < aplNELMSub);

    // Skip over the header and dimensions to the data
    lpMemSub = VarArrayDataFmBase (lpMemSub);

    // Get next item from global memory
    GetNextItemMem (lpMemSub,               // Ptr to item global memory data
                    aplTypeSub,             // Item storage type
                    aplNELMSub,             // Item NELM
                    uSub,                   // Index into item
                    lphGlbRes,              // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    lpaplLongestRes);       // Ptr to result immediate value (may be NULL)
    // We no longer need this ptr
    MyGlobalUnlock (hGlbSub); lpMemSub = NULL;
} // End GetNextItemGlb


//***************************************************************************
//  $GetNextValueGlb
//
//  Get next value from a global memory handle
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextValueGlb
    (HGLOBAL     hGlbSub,                   // Item global memory handle
     APLUINT     uSub,                      // Index into item
     HGLOBAL    *lphGlbRes,                 // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes,           // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

{
    APLSTYPE  aplTypeSub;                   // Item storage type
    APLNELM   aplNELMSub;                   // Item NELM
    APLRANK   aplRankSub;                   // Item rank
    LPVOID    lpMemSub;                     // Ptr to item global memory

    // Lock the memory to get a ptr to it
    lpMemSub = MyGlobalLock (hGlbSub);

    // Split cases based upon the signature
    switch (GetSignatureMem (lpMemSub))
    {
        case LSTARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPLSTARRAY_HEADER) lpMemSub)
            // Get the Array Type and Rank
            aplTypeSub = ARRAY_LIST;
            aplNELMSub = lpHeader->NELM;
            aplRankSub = 1;
#undef  lpHeader
            // Skip over the header and dimensions to the data
            lpMemSub = LstArrayBaseToData (lpMemSub);

            break;

        case VARARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPVARARRAY_HEADER) lpMemSub)
            // Get the Array Type and Rank
            aplTypeSub = lpHeader->ArrType;
            aplNELMSub = lpHeader->NELM;
            aplRankSub = lpHeader->Rank;
#undef  lpHeader
            // Skip over the header and dimensions to the data
            lpMemSub = VarArrayDataFmBase (lpMemSub);

            break;

        case DFN_HEADER_SIGNATURE:
        case FCNARRAY_HEADER_SIGNATURE:
        defstop
            break;
    } // End SWITCH

    Assert (aplNELMSub EQ 0
         || (uSub < aplNELMSub)
         || ((uSub EQ 0) && IsNested (aplTypeSub)));

    // Get next value from global memory
    GetNextValueMem (lpMemSub,              // Ptr to item global memory data
                     aplTypeSub,            // Item storage type
                     aplNELMSub,            // Item NELM
                     uSub,                  // Index into item
                     lphGlbRes,             // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                     lpaplLongestRes,       // Ptr to result immediate value (may be NULL)
                     lpimmTypeRes);         // Ptr to result immediate type (may be NULL)
    // We no longer need this ptr
    MyGlobalUnlock (hGlbSub); lpMemSub = NULL;
} // End GetNextValueGlb


//***************************************************************************
//  $GetNextValueMem
//
//  Get next value from global memory
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextValueMem
    (LPVOID      lpMemSub,                  // Ptr to item global memory data
     APLSTYPE    aplTypeSub,                // Item storage type
     APLNELM     aplNELMSub,                // Item NELM
     APLUINT     uSub,                      // Index into item
     HGLOBAL    *lphGlbRes,                 // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes,           // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

{
    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
                        aplNELMSub,         // Item NELM
                        uSub,               // Index into item
                        lphGlbRes,          // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                        lpaplLongestRes,    // Ptr to result immediate value (may be NULL)
                        lpimmTypeRes);      // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
} // End GetNextValueMem


//***************************************************************************
//  $GetNextItemMem
//
//  Get next item from global memory
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextItemMem
    (LPVOID      lpMemSub,              // Ptr to item global memory data
     APLSTYPE    aplTypeSub,            // Item storage type
     APLSTYPE    aplNELMSub,            // Item NELM
     APLUINT     uSub,                  // Index into item
     HGLOBAL    *lphGlbRes,             // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes)       // Ptr to result immediate value (may be NULL)

{
    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
                        aplNELMSub,         // Item NELM
                        uSub,               // Index into item
                        lphGlbRes,          // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                        lpaplLongestRes,    // Ptr to result immediate value (may be NULL)
                        NULL);              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
} // End GetNextItemMem


//***************************************************************************
//  $GetNextSimple
//
//  Get next simple (or GlbNum) item from global memory
//***************************************************************************

void GetNextSimple
    (LPVOID      lpMemSub,                  // Ptr to item global memory data
     APLSTYPE    aplTypeSub,                // Item storage type
     APLSTYPE    aplNELMSub,                // Item NELM
     APLUINT     uSub,                      // Index into item
     LPAPLINT    lpaplInteger,              // Ptr to Boolean/Integer result
     LPAPLFLOAT  lpaplFloat,                // Ptr to Float result
     LPAPLCHAR   lpaplChar,                 // Ptr to Char result
     HGLOBAL    *lplpSymGlb)                // Ptr to global memory handle (may be NULL)

{
    APLLONGEST aplLongest;                  // Ptr to result immediate value (may be NULL)

    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
                        aplNELMSub,         // Item NELM
                        uSub,               // Index into item
                        lplpSymGlb,         // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                       &aplLongest,         // Ptr to result immediate value (may be NULL)
                        NULL);              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    if (lpaplInteger NE NULL)
       *lpaplInteger = aplLongest;
    if (lpaplFloat NE NULL)
       *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
    if (lpaplChar NE NULL)
       *lpaplChar    = (APLCHAR) aplLongest;
} // End GetNextSimple


//***************************************************************************
//  $GetNextValueMemSub
//
//  Get next value from global memory
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//  If both lphGlbRes and lpimmTypeRes are not NULL, *lphGlbRes
//    is never an LPSYMENTRY; instead *lpimmTypeRes is filled in
//    with the immediate type.
//***************************************************************************

void GetNextValueMemSub
    (LPVOID      lpMemSub,              // Ptr to item global memory data
     APLSTYPE    aplTypeSub,            // Item storage type
     APLNELM     aplNELMSub,            // Item NELM
     APLUINT     uSub,                  // Index into item
     HGLOBAL    *lphGlbRes,             // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes,       // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)          // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

{
    APLHETERO lpSymSub;                 // Item as APLHETERO
    LPAPLLIST lptkList;                 // Item as APLLIST

    // Assume the result is an immediate
    if (lphGlbRes NE NULL)
        *lphGlbRes = NULL;

    // If the item is empty,
    //   and not a ptr array, ...
    if (IsEmpty (aplNELMSub)
     && !IsPtrArray (aplTypeSub))
    {
        APLRAT aplRat = {0};
        APLVFP aplVfp = {0};
        APLARB aplArb = {0};

        Assert (uSub EQ 0);

        // Split cases based upon the right arg storage type
        switch (aplTypeSub)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_HC2I:
            case ARRAY_HC2F:
            case ARRAY_HC4I:
            case ARRAY_HC4F:
            case ARRAY_HC8I:
            case ARRAY_HC8F:
            case ARRAY_APA:
            case ARRAY_LIST:
                if (lpaplLongestRes NE NULL)
                    *lpaplLongestRes = 0;
                if (lpimmTypeRes NE NULL)
                    *lpimmTypeRes    = IMMTYPE_BOOL;
                break;

            case ARRAY_CHAR:
                if (lpaplLongestRes NE NULL)
                    *lpaplLongestRes = L' ';
                if (lpimmTypeRes NE NULL)
                    *lpimmTypeRes    = IMMTYPE_CHAR;
                break;

            case ARRAY_RAT:
                // Initialize to 0/1
                mpq_init (&aplRat);

                if (lphGlbRes NE NULL)
                {
                    *lphGlbRes       =
                      MakeGlbEntry_EM (aplTypeSub,                      // Entry type
                                      &aplRat,                          // Ptr to the value
                                       FALSE,                           // TRUE iff we should initialize the target first
                                       NULL);                           // Ptr to function token
                    if (*lphGlbRes EQ NULL)
                        goto ERROR_EXIT;
                } // End IF

                if (lpimmTypeRes NE NULL)
                    *lpimmTypeRes    = IMMTYPE_RAT;
                break;

            case ARRAY_VFP:
                // Initialize to 0
                mpfr_init0 (&aplVfp);

                if (lphGlbRes NE NULL)
                {
                    *lphGlbRes       =
                      MakeGlbEntry_EM (aplTypeSub,                      // Entry type
                                      &aplVfp,                          // Ptr to the value
                                       FALSE,                           // TRUE iff we should initialize the target first
                                       NULL);                           // Ptr to function token
                    if (*lphGlbRes EQ NULL)
                        goto ERROR_EXIT;
                } // End IF

                if (lpimmTypeRes NE NULL)
                    *lpimmTypeRes    = IMMTYPE_VFP;
                break;

            case ARRAY_ARB:
                // Initialize to 0/0
                Myarb_init (&aplArb);

                if (lphGlbRes NE NULL)
                {
                    *lphGlbRes       =
                      MakeGlbEntry_EM (aplTypeSub,                      // Entry type
                                      &aplArb,                          // Ptr to the value
                                       FALSE,                           // TRUE iff we should initialize the target first
                                       NULL);                           // Ptr to function token
                    if (*lphGlbRes EQ NULL)
                        goto ERROR_EXIT;
                } // End IF

                if (lpimmTypeRes NE NULL)
                    *lpimmTypeRes    = IMMTYPE_ARB;
                break;

            defstop
                break;
        } // End SWITCH
    } else
    // Split cases based upon the right arg storage type
    switch (aplTypeSub)
    {
        case ARRAY_BOOL:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = BIT0 & (((LPAPLBOOL) lpMemSub)[uSub >> LOG2NBIB] >> (MASKLOG2NBIB & uSub));
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_BOOL;
            break;

        case ARRAY_INT:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = ((LPAPLINT) lpMemSub)[uSub];
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_INT;
            break;

        case ARRAY_FLOAT:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = *(LPAPLLONGEST) &((LPAPLFLOAT) lpMemSub)[uSub];
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_FLOAT;
            break;

        case ARRAY_CHAR:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = ((LPAPLCHAR) lpMemSub)[uSub];
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_CHAR;
            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemSub)
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = lpAPA->Off + lpAPA->Mul * uSub;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_INT;
#undef  lpAPA
            break;

        case ARRAY_LIST:
            lptkList = &((LPAPLLIST) lpMemSub)[uSub];

            // Split cases based upon the token type
            switch (lptkList->tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Extract the immediate type & value
                    if (lpaplLongestRes NE NULL)
                        *lpaplLongestRes = *GetPtrTknLongest (lptkList);
                    if (lpimmTypeRes NE NULL)
                        *lpimmTypeRes    = lptkList->tkFlags.ImmType;
                    break;

                case TKT_VARARRAY:
                    if (lpaplLongestRes NE NULL)
                        *lpaplLongestRes = 0;
                    if (lpimmTypeRes NE NULL)
                    {
                        APLSTYPE aplTypeSub;

                        // Get the global attrs
                        AttrsOfGlb (lptkList->tkData.tkGlbData, &aplTypeSub, NULL, NULL, NULL);

                        *lpimmTypeRes    = TranslateArrayTypeToImmType (aplTypeSub);;
                    } // End IF

                    if (lphGlbRes NE NULL)
                        *lphGlbRes       = lptkList->tkData.tkGlbData;
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Get the item
            lpSymSub = ((LPAPLNESTED) lpMemSub)[uSub];

            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (lpSymSub))
            {
                case PTRTYPE_STCONST:
                    // Extract the immediate type & value
                    if (lpaplLongestRes NE NULL)
                        *lpaplLongestRes = lpSymSub->stData.stLongest;
                    if (lpimmTypeRes NE NULL)
                        *lpimmTypeRes    = lpSymSub->stFlags.ImmType;
                    break;

                case PTRTYPE_HGLOBAL:
                    if (lphGlbRes NE NULL)
                        *lphGlbRes       = lpSymSub;
                    if (lpimmTypeRes NE NULL)
                        *lpimmTypeRes    = GetImmTypeGlb (lpSymSub);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_RAT:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_RAT;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLRAT) lpMemSub)[uSub];
            break;

        case ARRAY_VFP:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_VFP;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLVFP) lpMemSub)[uSub];
            break;

        case ARRAY_ARB:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_ARB;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLARB) lpMemSub)[uSub];
            break;

        case ARRAY_HC2I:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC2I;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC2I) lpMemSub)[uSub];
            break;

        case ARRAY_HC2F:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = *(LPAPLLONGEST) &((LPAPLHC2F) lpMemSub)[uSub];
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC2F;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC2F) lpMemSub)[uSub];
            break;

        case ARRAY_HC2R:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC2R;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC2R) lpMemSub)[uSub];
            break;

        case ARRAY_HC2V:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC2V;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC2V) lpMemSub)[uSub];
            break;

        case ARRAY_HC4I:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC4I;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC4I) lpMemSub)[uSub];
            break;

        case ARRAY_HC4F:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = *(LPAPLLONGEST) &((LPAPLHC4F) lpMemSub)[uSub];
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC4F;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC4F) lpMemSub)[uSub];
            break;

        case ARRAY_HC4R:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC4R;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC4R) lpMemSub)[uSub];
            break;

        case ARRAY_HC4V:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC4V;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC4V) lpMemSub)[uSub];
            break;

        case ARRAY_HC8I:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC8I;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC8I) lpMemSub)[uSub];
            break;

        case ARRAY_HC8F:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = *(LPAPLLONGEST) &((LPAPLHC8F) lpMemSub)[uSub];
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC8F;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC8F) lpMemSub)[uSub];
            break;

        case ARRAY_HC8R:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC8R;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC8R) lpMemSub)[uSub];
            break;

        case ARRAY_HC8V:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_HC8V;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLHC8V) lpMemSub)[uSub];
            break;

        case ARRAY_BA2F:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_BA2F;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLBA2F) lpMemSub)[uSub];
            break;

        case ARRAY_BA4F:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_BA4F;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLBA4F) lpMemSub)[uSub];
            break;

        case ARRAY_BA8F:
            if (lpaplLongestRes NE NULL)
                *lpaplLongestRes = 0;
            if (lpimmTypeRes NE NULL)
                *lpimmTypeRes    = IMMTYPE_BA8F;
            if (lphGlbRes NE NULL)
                *lphGlbRes       = &((LPAPLBA8F) lpMemSub)[uSub];
            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    return;

ERROR_EXIT:
    if (lpaplLongestRes NE NULL)
        *lpaplLongestRes = 0;
    if (lpimmTypeRes NE NULL)
        *lpimmTypeRes    = IMMTYPE_ERROR;
    if (lphGlbRes NE NULL)
        *lphGlbRes       = NULL;
} // End GetNextValueMemSub


//***************************************************************************
//  $GetGlbHandle
//
//  Return the HGLOBAL from a token if it's an HGLOBAL
//    NULL otherwise.
//***************************************************************************

HGLOBAL GetGlbHandle
    (LPTOKEN lpToken)           // Ptr to token

{
    // If it's an immediate object, ...
    if (IsTknImmed (lpToken))
        return NULL;
    else
        return GetGlbDataToken (lpToken);
} // End GetGlbHandle


//***************************************************************************
//  $SetGlbHandle
//
//  Save a new HGLOBAL into a token if it's an HGLOBAL
//    NULL otherwise.
//***************************************************************************

UBOOL SetGlbHandle
    (LPTOKEN lpToken,           // Ptr to token
     HGLOBAL hGlbSrc)           // Source global memory handle

{
    // If it's an immediate object, ...
    if (IsTknImmed (lpToken))
        return FALSE;
    else
        return SetGlbDataToken (lpToken, hGlbSrc);
} // End SetGlbHandle


//***************************************************************************
//  $GetGlbPtrs
//
//  Return the HGLOBAL and LPVOID from a token if it's an HGLOBAL,
//    NULL otherwise.
//  If the arg is immediate, return the APLLONGEST value.
//***************************************************************************

APLLONGEST GetGlbPtrs
    (LPTOKEN  lpToken,          // Ptr to token
     HGLOBAL *lphGlb)           // Ptr to ptr to HGLOBAL

{
    // Get the global handle & longest w/o locking it
    return GetGlbPtrs_LOCK (lpToken, lphGlb, NULL);
} // End GetGlbPtrs


//***************************************************************************
//  $GetGlbPtrs_LOCK
//
//  Return the HGLOBAL and LPVOID from a token if it's an HGLOBAL,
//    NULL otherwise.
//  If the arg is immediate, return the APLLONGEST value.
//
//  The "_LOCK" suffix is a reminder that this function locks
//    a global memory handle and the caller must unlock it.
//***************************************************************************

APLLONGEST GetGlbPtrs_LOCK
    (LPTOKEN  lpToken,          // Ptr to token
     HGLOBAL *lphGlb,           // Ptr to ptr to HGLOBAL
     LPVOID  *lplpMemHdr)       // Ptr to ptr to header (may be NULL)

{
    APLLONGEST aplLongest;          // Value if immediate

    // Get and lock the global ptrs
    GetGlbMem_LOCK (lpToken,        // Ptr to token
                    lphGlb,         // ...    ptr to HGLOBAL
                    lplpMemHdr,     // ...           header (may be NULL)
                   &aplLongest);    // ...    aplLongest (may be NULL)
    return aplLongest;
} // End GetGlbPtrs_LOCK


//***************************************************************************
//  $GetGlbMem_LOCK
//
//  Return the HGLOBAL and LPVOID from a token if it's an HGLOBAL,
//    NULL otherwise.
//  If the arg is immediate, return the APLLONGEST value.
//
//  The "_LOCK" suffix is a reminder that this function locks
//    a global memory handle and the caller must unlock it.
//***************************************************************************

LPVOID GetGlbMem_LOCK
    (LPTOKEN      lpToken,      // Ptr to token
     HGLOBAL     *lphGlb,       // ...    ptr to HGLOBAL
     LPVOID      *lplpMemHdr,   // ...           header (may be NULL)
     LPAPLLONGEST lpaplLongest) // ...    aplLongest (may be NULL)

{
    LPVOID     lpMemHdr = NULL; // Ptr to locked memory header
    APLSTYPE   aplTypeMem;      // Memory storage type
    APLNELM    aplNELMMem;      // Memory NELM
    APLLONGEST aplLongest = 0;  // Value if immediate

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:      // Needed when called from <pf_index.c> on target of assignment
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Return the global memory handle
                *lphGlb = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (*lphGlb));

                // Lock the memory to get a ptr to it
                lpMemHdr = MyGlobalLockVar (*lphGlb);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemHdr)
                // Get the type & NELM
                aplTypeMem = lpHeader->ArrType;
                aplNELMMem = lpHeader->NELM;
#undef  lpHeader
                break;      // Continue with common HGLOBAL code
            } // End IF

            // Handle the immediate case
            *lphGlb  = NULL;

            // If the caller wants the locked memory ptr, ...
            if (lplpMemHdr NE NULL)
                *lplpMemHdr = NULL;

            // Save the value of the longest
            aplLongest = lpToken->tkData.tkSym->stData.stLongest;

            break;

        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Return the global memory handle
                *lphGlb = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir_PTB (*lphGlb)
                     || IsGlbTypeDfnDir_PTB (*lphGlb));
                // Split cases based upon the type
                if (IsGlbTypeFcnDir_PTB (*lphGlb))
                    // Lock the memory to get a ptr to it
                    lpMemHdr = MyGlobalLockFcn (*lphGlb);
                else
                if (IsGlbTypeDfnDir_PTB (*lphGlb))
                    // Lock the memory to get a ptr to it
                    lpMemHdr = MyGlobalLockDfn (*lphGlb);
#ifdef DEBUG
                else
                    DbgBrk ();
#endif

                // Get the pseudo-type & NELM
                aplTypeMem = ARRAY_LIST;
                aplNELMMem = 0;

                break;      // Continue with common HGLOBAL code
            } // End IF

            // Handle the immediate case
            *lphGlb  = NULL;

            // If the caller wants the locked memory ptr, ...
            if (lplpMemHdr NE NULL)
                *lplpMemHdr = NULL;

            // Save the value of the longest
            aplLongest = lpToken->tkData.tkSym->stData.stLongest;

            break;

        case TKT_FCNAFO:
        case TKT_DELAFO:
        case TKT_DELDEL:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
        case TKT_FCNDFN:
        case TKT_OP1DFN:
        case TKT_OP2DFN:
            // Return the global memory handle
            *lphGlb = lpToken->tkData.tkGlbData;

            // stData is a valid HGLOBAL function array
            Assert (IsGlbTypeDfnDir_PTB (*lphGlb));

            // Lock the memory to get a ptr to it
            lpMemHdr = MyGlobalLockDfn (*lphGlb);

            // Get the pseudo-type & NELM
            aplTypeMem = ARRAY_LIST;
            aplNELMMem = 0;

            break;      // Continue with common HGLOBAL code

        case TKT_VARIMMED:
        case TKT_AXISIMMED:
        case TKT_LSTIMMED:
        case TKT_FCNIMMED:
            // Handle the immediate case
            *lphGlb  = NULL;

            // If the caller wants the locked memory ptr, ...
            if (lplpMemHdr NE NULL)
                *lplpMemHdr = NULL;

            // Save the value of the longest
            aplLongest = *GetPtrTknLongest (lpToken);

            break;

        case TKT_FCNARRAY:
            // Return the global memory handle
            *lphGlb = lpToken->tkData.tkGlbData;

            // Lock the memory to get a ptr to it
            lpMemHdr = MyGlobalLockFcn (*lphGlb);

            // Get the pseudo-type & NELM
            aplTypeMem = ARRAY_LIST;
            aplNELMMem = 0;

            break;      // Continue with common HGLOBAL code

        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARARRAY:
        case TKT_AXISARRAY:
            // Return the global memory handle
            *lphGlb = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (*lphGlb));

            // Lock the memory to get a ptr to it
            lpMemHdr = MyGlobalLockVar (*lphGlb);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemHdr)
            // Get the type & NELM
            aplTypeMem = lpHeader->ArrType;
            aplNELMMem = lpHeader->NELM;
#undef  lpHeader
            break;      // Continue with common HGLOBAL code

        case TKT_LSTARRAY:
        case TKT_LSTMULT:
            // Return the global memory handle
            *lphGlb = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL list array
            Assert (IsGlbTypeLstDir_PTB (*lphGlb));

            // Lock the memory to get a ptr to it
            lpMemHdr = MyGlobalLockLst (*lphGlb);

#define lpHeader        ((LPLSTARRAY_HEADER) lpMemHdr)
            // Get the type & NELM
            aplTypeMem = ARRAY_LIST;
            aplNELMMem = lpHeader->NELM;
#undef  lpHeader
            break;      // Continue with common HGLOBAL code

        defstop
            break;
    } // End SWITCH

    // If the arg is non-empty & not a list, ...
    if (!IsEmpty (aplNELMMem)
     && !IsList (aplTypeMem)
     && *lphGlb NE NULL)
        GetFirstValueGlb (*lphGlb,          // The global memory handle
                           NULL,            // Ptr to integer (or Boolean) (may be NULL)
                           NULL,            // ...    float (may be NULL)
                           NULL,            // ...    char (may be NULL)
                          &aplLongest,      // ...    longest (may be NULL)
                           NULL,            // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                           NULL,            // ...    immediate type (see IMM_TYPES) (may be NULL)
                           NULL,            // ...    array type -- ARRAY_TYPES (may be NULL)
                           TRUE);           // TRUE iff we should expand LPSYMENTRY into immediate value
    // If the caller wants the longest, ...
    if (lpaplLongest NE NULL)
        *lpaplLongest = aplLongest;

    // If the caller wants the locked memory ptr, ...
    if (lplpMemHdr NE NULL
     &&   lpMemHdr NE NULL)
    {
        // The memory is already locked
        *lplpMemHdr = lpMemHdr;

        // Return a ptr to the global memory data
        return VarArrayDataFmBase (*lplpMemHdr);
    } else
    {
        if (*lphGlb NE NULL
         && lpMemHdr NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (*lphGlb); lpMemHdr = NULL;
        } // End IF

        // Return a ptr to the global memory data
        return lpaplLongest;
    } // End IF/ELSE
} // End GetGlbMem_LOCK


//***************************************************************************
//  $GetQuadCT
//
//  Get the current value of []CT
//***************************************************************************

APLFLOAT GetQuadCT
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_CT]->stData.stFloat;
} // End GetQuadCT


//***************************************************************************
//  $SetQuadCT
//
//  Set the current value of []CT
//***************************************************************************

void SetQuadCT
    (APLFLOAT fQuadCT)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_CT]->stData.stFloat = fQuadCT;
} // End SetQuadCT


//***************************************************************************
//  $GetQuadFPC
//
//  Get the current value of []FPC
//***************************************************************************

APLUINT GetQuadFPC
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_FPC]->stData.stInteger;
} // End GetQuadFPC


//***************************************************************************
//  $GetQuadFPC_PTD
//
//  Get the current value of []FPC
//***************************************************************************

APLUINT GetQuadFPC_PTD
    (LPPERTABDATA lpMemPTD)

{
    return lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_FPC]->stData.stInteger;
} // End GetQuadFPC_PTD


//***************************************************************************
//  $SetQuadFPC
//
//  Set the current value of []FPC
//***************************************************************************

void SetQuadFPC
    (APLUINT uQuadFPC)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_FPC]->stData.stInteger = uQuadFPC;
} // End SetQuadFPC


//***************************************************************************
//  $GetQuadLR
//
//  Get the current value of []LR
//***************************************************************************

APLCHAR GetQuadLR
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_LR]->stData.stChar;
} // End GetQuadLR


//***************************************************************************
//  $SetQuadLR
//
//  Set the current value of []LR
//***************************************************************************

void SetQuadLR
    (APLCHAR cQuadLR)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_LR]->stData.stChar = tolower (cQuadLR);
} // End SetQuadLR


//***************************************************************************
//  $GetQuadDT
//
//  Get the current value of []DT
//***************************************************************************

APLCHAR GetQuadDT
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_DT]->stData.stChar;
} // End GetQuadDT


//***************************************************************************
//  $SetQuadDT
//
//  Set the current value of []DT
//***************************************************************************

void SetQuadDT
    (APLCHAR cQuadDT)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_DT]->stData.stChar = tolower (cQuadDT);
} // End SetQuadDT


//***************************************************************************
//  $GetQuadIO
//
//  Get the current value of []IO
//***************************************************************************

APLBOOL GetQuadIO
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_IO]->stData.stBoolean;
} // End GetQuadIO


//***************************************************************************
//  $SetQuadIO
//
//  Set the current value of []IO
//***************************************************************************

void SetQuadIO
    (APLBOOL bQuadIO)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_IO]->stData.stBoolean = bQuadIO;
} // End SetQuadIO


//***************************************************************************
//  $GetQuadPP
//
//  Get the current value of []PP
//***************************************************************************

APLUINT GetQuadPP
    (void)

{
    return min (DEF_MAX_QUADPP_IEEE, GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger);
} // End GetQuadPP


//***************************************************************************
//  $SetQuadPPV
//
//  Set the current value of []PP for VFPs and ARBs
//***************************************************************************

void SetQuadPPV
    (APLINT uQuadPPV)

{
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger=
      min (DEF_MAX_QUADPP_VFP, uQuadPPV);
} // End SetQuadPPV


//***************************************************************************
//  $GetQuadPPV
//
//  Get the current value of []PP for VFP and ARB
//***************************************************************************

APLUINT GetQuadPPV
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger;
} // End GetQuadPPV


//***************************************************************************
//  $GetQuadPW
//
//  Get the current value of []PW
//***************************************************************************

APLUINT GetQuadPW
    (void)

{
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_PW]->stData.stInteger;
} // End GetQuadPW


//***************************************************************************
//  $GetQuadRL
//
//  Get the current value of []RL
//***************************************************************************

APLUINT GetQuadRL
    (void)

{
    // Get the current value
    return GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_RL]->stData.stInteger;
} // End GetQuadRL


//***************************************************************************
//  $SetQuadRL
//
//  Set the current value of []RL
//***************************************************************************

void SetQuadRL
    (APLUINT uQuadRL)           // []RL

{
    // Set the new value
    GetMemPTD ()->lphtsPTD->lpSymQuad[SYSVAR_RL]->stData.stInteger = uQuadRL;
} // End SetQuadRL


//***************************************************************************
//  $SavePrimSpecRL
//
//  Save []RL in lpPrimSpec
//***************************************************************************

void SavePrimSpecRL
    (LPPRIMSPEC lpPrimSpec)

{
    // Save it
    lpPrimSpec->uQuadRL = GetQuadRL ();
} // End SavePrimSpecRL


//***************************************************************************
//  $RestPrimSpecRL
//
//  Restore []RL from lpPrimSpec
//***************************************************************************

void RestPrimSpecRL
    (LPPRIMSPEC lpPrimSpec)

{
    APLUINT      uQuadRL;           // []RL for atomicity
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the new value
    uQuadRL = lpPrimSpec->uQuadRL;

    // If there's something new to store, ...
    if (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_RL]->stData.stInteger NE uQuadRL)
    {
        LPSIS_HEADER lpSISCur;          // Ptr to current SIS_HEADER srtuc

        // Save the new value
        lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_RL]->stData.stInteger = uQuadRL;

        // Loop backwards through the SI levels
        for (lpSISCur = lpMemPTD->lpSISCur;
             lpSISCur NE NULL;
             lpSISCur = lpSISCur->lpSISPrv)
        {
            // If this is a function or operator, ...
            if (lpSISCur->DfnType EQ DFNTYPE_OP1
             || lpSISCur->DfnType EQ DFNTYPE_OP2
             || lpSISCur->DfnType EQ DFNTYPE_FCN)
            {
                // If []RL is not local, ...
                if (!lpSISCur->bLclRL)
                {
                    // If the ptr to the previous HASHTABSTR is valid, ...
                    if (lpSISCur->lphtsPrv NE NULL)
                        // Pass the new value up to the next layer
                        lpSISCur->lphtsPrv->lpSymQuad[SYSVAR_RL]->stData.stInteger = uQuadRL;
                } else
                    break;
            } // End IF
        } // End FOR
    } // End IF
} // End RestPrimSpecRL


//***************************************************************************
//  $GetLftFcnTkn
//
//  Return a ptr to the left function token from a PL_YYSTYPE.
//***************************************************************************

LPTOKEN GetLftFcnTkn
    (LPPL_YYSTYPE lpYYFcnStrOpr)

{
    LPPL_YYSTYPE lpYYFcnStrLft;     // Ptr to left operand function strand (may be NULL if not an operator)
    LPTOKEN      lptkAxisOpr;       // Ptr to operator's axis token (may be NULL)

    // Split cases based upon the token type
    switch (lpYYFcnStrOpr->tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
            // If it's DoubleTilde, ...
            if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DOUBLETILDE)
            {
                // Check for axis operator
                lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

                // Set ptr to left operand
                lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr); Assert (lpYYFcnStrLft NE NULL);

                // If it's valid, ...
                if (lpYYFcnStrLft NE NULL)
                // Split cases based upon the immediate function
                switch (lpYYFcnStrLft->tkToken.tkData.tkChar)
                {
                    case UTF16_UPSHOE:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_INTER;

                        break;

                    case UTF16_DOWNSHOE:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_UNION;

                        break;

                    case UTF16_LEFTCARET:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_LT;

                        break;

                    case UTF16_LEFTCARETUNDERBAR:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_LE;

                        break;

                    case UTF16_EQUAL:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_EQ;

                        break;

                    case UTF16_RIGHTCARETUNDERBAR:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_GE;

                        break;

                    case UTF16_RIGHTCARET:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_GT;

                        break;

                    case UTF16_NOTEQUAL:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_NE;

                        break;

                    case UTF16_LEFTSHOE:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_LS;

                        break;

                    case UTF16_RIGHTSHOE:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_RS;

                        break;

                    case UTF16_JOT:
                        lpYYFcnStrLft->tkToken.tkData.tkChar = INDEX_DOUBLETILDE_JOT;

                        break;

                    case INDEX_DOUBLETILDE_INTER:
                    case INDEX_DOUBLETILDE_UNION:
                    case INDEX_DOUBLETILDE_LT:
                    case INDEX_DOUBLETILDE_LE:
                    case INDEX_DOUBLETILDE_EQ:
                    case INDEX_DOUBLETILDE_GE:
                    case INDEX_DOUBLETILDE_GT:
                    case INDEX_DOUBLETILDE_NE:
                    case INDEX_DOUBLETILDE_LS:
                    case INDEX_DOUBLETILDE_RS:
                    case INDEX_DOUBLETILDE_JOT:
                        break;

                    default:
                        break;
                } // End IF/SWITCH

                return &lpYYFcnStrLft->tkToken;
            } // End IF

            // Fall through to common code

        default:
            return &lpYYFcnStrOpr->tkToken;
    } // End SWITCH
} // End GetLftFcnTkn


//***************************************************************************
//  $GetPrototypeFcnPtr
//
//  Return a ptr to the appropriate prototype function
//    corresponding to a given function token type.
//***************************************************************************

LPPRIMFNS GetPrototypeFcnPtr
    (LPTOKEN lptkFunc)

{
    HGLOBAL      hGlbFcn;                   // Function array global memory handle
    LPPL_YYSTYPE lpMemFcn;                  // Ptr to function array global memory
    LPPRIMFNS    lpPrimFns;                 // Ptr to result

    // Split cases based upon the token type of the function strand's first item
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            // Get a ptr to the prototype function for the first symbol (a function)
            return PrimProtoFnsTab[SymTrans (lptkFunc)];

        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
            // Note the white lie we tell here (LPPRIMFNS) instead of
            //   (LPPRIMOPS) so we can use one function (GetPrototypeFcnPtr)
            //   for both primitive functions and operators.  When the
            //   prototype function for a primitive function is called,
            //   lptkFunc is a ptr to the primitive function token.
            //   When the prototype function for a primitive operator
            //   is called, the same parameter is a ptr to the function strand.
            // Get a ptr to the prototype function for the first symbol (an operator)
            return (LPPRIMFNS) PrimProtoOpsTab[SymTrans (lptkFunc)];

        case TKT_FCNAFO:        // Handle as any other function
        case TKT_OP1AFO:        // ...
        case TKT_OP2AFO:        // ...
        case TKT_FCNDFN:        // ...
        case TKT_OP1DFN:        // ...
        case TKT_OP2DFN:        // ...
        case TKT_FCNNAMED:      // e.g. []VR{each}0{rho}{enclose}'abc'
        case TKT_OP1NAMED:      // ...
        case TKT_OP2NAMED:      // ...
            // If it's an internal function, ...
            if (lptkFunc->tkData.tkSym->stFlags.FcnDir)
                return NULL;

            // Fall through to unnamed behavior

        case TKT_FCNARRAY:
            // Get the function global memory handle
            hGlbFcn = GetGlbHandle (lptkFunc);

            // Split cases based upon the function array signature
            switch (GetSignatureGlb_PTB (hGlbFcn))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Lock the memory to get a ptr to it
                    lpMemFcn = MyGlobalLockFcn (hGlbFcn);

                    // Skip over the header to the data
                    lpMemFcn = FcnArrayBaseToData (lpMemFcn);

                    // Recurse to get the result
                    lpPrimFns = GetPrototypeFcnPtr (&lpMemFcn->tkToken);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

                    return lpPrimFns;

                case DFN_HEADER_SIGNATURE:
                    // Get a ptr to the prototype function for the User-defined function/operator
                    return &ExecDfnGlbProto_EM_YY;

                defstop
                    return NULL;
            } // End SWITCH

        defstop
            return NULL;
    } // End SWITCH
} // End GetPrototypeFcnPtr


//***************************************************************************
//  $GetPrimFlagsPtr
//
//  Return a ptr to the PrimFlags entry
//    corresponding to a given function token type.
//***************************************************************************

LPPRIMFLAGS GetPrimFlagsPtr
    (LPTOKEN lptkFunc)

{
    HGLOBAL      hGlbFcn;                   // Function array global memory handle
    LPPL_YYSTYPE lpMemFcn;                  // Ptr to function array global memory
    LPPRIMFLAGS  lpPrimFlags;               // Ptr to result
    static PRIMFLAGS
      DfnIdentFns =
    {
        {FALSE},                            // Flags
        (LPPRIMOPS) ExecDfnGlbIdent_EM_YY,  // Ptr to PRIMOPS
    },
      PrimFlagsNone = {0};

    // Split cases based upon the token type of the function strand's first item
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
        case TKT_FILLJOT:
            return &PrimFlags[SymTrans (lptkFunc)];

        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // If this is a direct function, ...
            if (lptkFunc->tkData.tkSym->stFlags.FcnDir)
                return &PrimFlagsNone;

            Assert (GetPtrTypeDir (lptkFunc->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Get the operator global memory handle
            hGlbFcn = lptkFunc->tkData.tkSym->stData.stGlbData;

            // Lock the memory to get a ptr to it
            lpMemFcn = MyGlobalLock (hGlbFcn);

            // Split cases based upon the signature
            switch (GetSignatureMem (lpMemFcn))
            {
                case DFN_HEADER_SIGNATURE:
                    // Get a ptr to the prototype function for the User-defined function/operator
                    lpPrimFlags = &DfnIdentFns;

                    break;

                case FCNARRAY_HEADER_SIGNATURE:
                    // Skip over the header to the data (PL_YYSTYPEs)
                    lpMemFcn = FcnArrayBaseToData (lpMemFcn);

                    // Get a ptr to the corresponding PrimFlags
                    lpPrimFlags = &PrimFlags[SymTrans (&lpMemFcn->tkToken)];

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

            return lpPrimFlags;

        case TKT_FCNARRAY:
            // Split cases based upon the function array signature
            switch (GetSignatureGlb (lptkFunc->tkData.tkGlbData))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Get the function global memory handle
                    hGlbFcn = lptkFunc->tkData.tkGlbData;

                    // Lock the memory to get a ptr to it
                    lpMemFcn = MyGlobalLockFcn (hGlbFcn);

                    // Skip over the header to the data
                    lpMemFcn = FcnArrayBaseToData (lpMemFcn);

                    // Recurse to get the result
                    lpPrimFlags = GetPrimFlagsPtr (&lpMemFcn->tkToken);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

                    return lpPrimFlags;

                case DFN_HEADER_SIGNATURE:
                    // Get a ptr to the prototype function for the User-defined function/operator
                    return &DfnIdentFns;

                defstop
                    return NULL;
            } // End SWITCH

            break;

        case TKT_DELAFO:
        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
        case TKT_FCNDFN:
        case TKT_OP1DFN:
        case TKT_OP2DFN:
            // Get a ptr to the prototype function for the user-defined function/operator
            return &DfnIdentFns;

        defstop
            return NULL;
    } // End SWITCH
} // End GetPrimFlagsPtr


//***************************************************************************
//  $GetImmedType
//
//  Get the immediate type from a token
//***************************************************************************

IMM_TYPES GetImmedType
    (LPTOKEN lptkArg)               // Ptr to token

{
    if (IsTknNamed (lptkArg))
        return lptkArg->tkData.tkSym->stFlags.ImmType;
    else
        return lptkArg->tkFlags.ImmType;
} // End GetImmedType


//***************************************************************************
//  $GetImmTypeGlb
//
//  Get the immediate type from a global
//***************************************************************************

IMM_TYPES GetImmTypeGlb
    (HGLOBAL hGlbArg)               // Ptr to token

{
    LPVARARRAY_HEADER lpMemArg;
    IMM_TYPES         immTypeArg;

    // Lock the memory to get a ptr to it
    lpMemArg = MyGlobalLockVar (hGlbArg);

    // Get the immediate type
    immTypeArg = TranslateArrayTypeToImmType (lpMemArg->ArrType);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemArg = NULL;

    return immTypeArg;
} // End GetImmTypeGlb


//***************************************************************************
//  $GetSignatureGlb
//
//  Get the signature of a global object
//***************************************************************************

UINT GetSignatureGlb
    (HGLOBAL hGlbLcl)

{
    LPHEADER_SIGNATURE lpMemLcl;            // Ptr to signature global memory
    UINT               Sig;                 // The signature

    // Lock the memory to get a ptr to it
    lpMemLcl = MyGlobalLock (hGlbLcl);

    Assert (IsValidPtr (lpMemLcl, sizeof (lpMemLcl->nature)));

    // Get the signature
    Sig = lpMemLcl->nature;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLcl); lpMemLcl = NULL;

    return Sig;
} // End GetSignatureGlb


//***************************************************************************
//  $GetSignatureGlb_PTB
//
//  Get the signature of a global object whose value is sensitive to Ptr Type Bits
//***************************************************************************

UINT GetSignatureGlb_PTB
    (LPVOID lpSymGlbLcl)

{
    LPHEADER_SIGNATURE lpMemLcl;            // Ptr to signature global memory
    UINT               Sig;                 // The signature
    HGLOBAL            hGlbLcl;             // Global memory handle
#ifdef DEBUG
    LPSYMENTRY         lpSymEntry = lpSymGlbLcl;
#else
  #define lpSymEntry    ((LPSYMENTRY) lpSymGlbLcl)
#endif

    // Split cases based upon the ptr bits
    switch (GetPtrTypeDir (lpSymGlbLcl))
    {
        case PTRTYPE_STCONST:
            // If it's a direct function, ...
            if (lpSymEntry->stFlags.FcnDir)
                // Return a pseudo-signature
                return SYSFN_HEADER_SIGNATURE;

            Assert (lpSymEntry->stFlags.Imm EQ FALSE);

            // Copy the HGLOBAL
            hGlbLcl = lpSymEntry->stData.stGlbData;

            break;

        case PTRTYPE_HGLOBAL:
            // Copy the HGLOBAL
            hGlbLcl = lpSymGlbLcl;

            break;

        defstop
            break;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpMemLcl = MyGlobalLock (hGlbLcl);

    // Get the signature
    Sig = lpMemLcl->nature;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLcl); lpMemLcl = NULL;

    return Sig;
#ifndef DEBUG
  #undef  lpSymEntry
#endif
} // End GetSignatureGlb_PTB


//***************************************************************************
//  $GetMemPTD
//
//  Return the current lpMemPTD
//***************************************************************************

LPPERTABDATA GetMemPTD
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory handle

    lpMemPTD = TlsGetValue (dwTlsPerTabData); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
    if (!IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
        lpMemPTD = GetPerTabPtr (TabCtrl_GetCurSel (hWndTC));
    Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    return lpMemPTD;
} // End GetMemPTD


//***************************************************************************
//  End of File: getfns.c
//***************************************************************************
