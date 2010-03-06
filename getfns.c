//***************************************************************************
//  NARS2000 -- Getxxx Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
    if (lpSymGlb)
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
        lptkRes->tkFlags.ImmType   = IMMTYPE_ERROR;
////////lptkRes->tkFlags.NoDisplay =                    // Filled in above
        lptkRes->tkData.tkGlbData  = hGlbSub;
////////lptkRes->tkCharIndex       =                    // Filled in above
    } // End IF/ELSE
} // End GetNextValueTokenIntoToken


//***************************************************************************
//  $GetNextValueTokenIntoNamedVarToken
//
//  Return the next value from a token as a VARNAMED.
//  The token may be an empty array in which case the
//    value of the prototype is returned.
//  Note that the nested global case increments the reference count
//***************************************************************************

void GetNextValueTokenIntoNamedVarToken
    (LPTOKEN      lptkArg,      // Ptr to the arg token
     APLUINT      uIndex,       // Index to use
     LPTOKEN      lptkRes)      // Ptr to the result token

{
    HGLOBAL    hGlbData,
               hGlbSub;
    IMM_TYPES  immType;

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

            return;

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
        lptkRes->tkData.tkSym->stFlags.ImmType    = IMMTYPE_ERROR;
        lptkRes->tkData.tkSym->stFlags.Value      = TRUE;
        lptkRes->tkData.tkSym->stFlags.ObjName    = OBJNAME_USR;
        lptkRes->tkData.tkSym->stFlags.stNameType = NAMETYPE_VAR;
        lptkRes->tkData.tkSym->stData.stGlbData   = CopySymGlbDir_PTB (hGlbSub);
////////lptkRes->tkCharIndex                      =     // Filled in above
    } // End IF/ELSE
} // End GetNextValueTokenIntoNamedVarToken


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
    if (lpSymGlb)
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
    if (lpaplLongest)
       *lpaplLongest = aplLongest;
    if (lpSymGlb)
       *lpSymGlb     = hGlbSub;
    if (lpImmType)
       *lpImmType    = immType;
    if (lpArrType)
       *lpArrType    = TranslateImmTypeToArrayType (immType);;

    // If the return value is immediate, ...
    if (hGlbSub EQ NULL)
    {
        if (lpaplInteger)
           *lpaplInteger = (APLINT) aplLongest;
        if (lpaplFloat)
           *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
        if (lpaplChar)
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
    if (lpSymGlb)
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
    if (lpImmType)
        *lpImmType    = immType;
    if (lpArrType)
        *lpArrType    = TranslateImmTypeToArrayType (immType);
    if (lpaplLongest)
        *lpaplLongest = aplLongest;

    // Split cases based upon the immediate type
    switch (immType)
    {
        case IMMTYPE_BOOL:
            if (lpaplInteger)
                *lpaplInteger = (APLBOOL) aplLongest;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) (APLBOOL) aplLongest;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            break;

        case IMMTYPE_INT:
            if (lpaplInteger)
                *lpaplInteger = (APLINT) aplLongest;
            if (lpaplFloat)
                // ***FIXME*** -- Possible loss of precision
                *lpaplFloat   = (APLFLOAT) (APLINT) aplLongest;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            break;

        case IMMTYPE_FLOAT:
            if (lpaplFloat)
                *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
            if (lpaplInteger)
                *lpaplInteger = (APLINT) *(LPAPLFLOAT) &aplLongest;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            break;

        case IMMTYPE_CHAR:
            if (lpaplInteger)
                *lpaplInteger = 0;
            if (lpaplFloat)
                *lpaplFloat   = 0;
            if (lpaplChar)
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

    // Clear the type bits in case they are set on the way in
    hGlbData = ClrPtrTypeDir (hGlbData);

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
            lpMem = VarArrayBaseToData (lpMem, lpHeader->Rank);
#undef  lpHeader
            break;

        case DFN_HEADER_SIGNATURE:
        case FCNARRAY_HEADER_SIGNATURE:
        defstop
    } // End SWITCH

    if (lpImmType)
        *lpImmType = TranslateArrayTypeToImmType (aplType);
    if (lpArrType)
        *lpArrType = aplType;

    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            // If the array is empty and numeric, the value must be Boolean or APA
            aplInteger = aplNELM ? (BIT0 & *(LPAPLBOOL) lpMem) : 0;

            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_INT:
            aplInteger = aplNELM ? *(LPAPLINT) lpMem : 0;

            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;  // ***FIXME*** -- Possible loss of precision
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMem)
            // If the array is empty and numeric, the value must be Boolean or APA
            aplInteger = aplNELM ? lpAPA->Off : 0;
#undef  lpAPA
            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;  // ***FIXME*** -- Possible loss of precision
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_FLOAT:
            aplFloat = aplNELM ? *(LPAPLFLOAT) lpMem : 0;

            if (lpaplFloat)
                *lpaplFloat   = aplFloat;
            if (lpaplInteger)
                *lpaplInteger = (APLINT) aplFloat;
            if (lpaplChar)
                *lpaplChar    = WC_EOS;
            if (lpaplLongest)
                *lpaplLongest = *(LPAPLLONGEST) &aplFloat;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_CHAR:
            // If the array is empty and char, the value must be blank
            aplChar = aplNELM ? (*(LPAPLCHAR) lpMem) : L' ';

            if (lpaplInteger)
                *lpaplInteger = 0;
            if (lpaplFloat)
                *lpaplFloat   = 0;
            if (lpaplChar)
                *lpaplChar    = aplChar;
            if (lpaplLongest)
                *lpaplLongest = aplChar;
            if (lpSymGlb)
                *lpSymGlb     = NULL;
            break;

        case ARRAY_LIST:
#define lptkList        ((LPAPLLIST) lpMem)
            // Split cases based upon the token type
            switch (lptkList->tkFlags.TknType)
            {
                case TKT_VARIMMED:
                    // Fill in default values
                    if (lpSymGlb)
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
                    if (lpaplInteger)
                        *lpaplInteger = 0;
                    if (lpaplFloat)
                        *lpaplFloat   = 0;
                    if (lpaplChar)
                        *lpaplChar    = WC_EOS;
                    if (lpaplLongest)
                        *lpaplLongest = WC_EOS;
                    if (lpSymGlb)
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
                    if (lpSymGlb)
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
                    if (lpaplInteger)
                        *lpaplInteger = 0;
                    if (lpaplFloat)
                        *lpaplFloat   = 0;
                    if (lpaplChar)
                        *lpaplChar    = WC_EOS;
                    if (lpaplLongest)
                        *lpaplLongest = 0;
                    if (lpSymGlb)
                        *lpSymGlb     = *(LPAPLNESTED) lpMem;
                    break;

                defstop
                    break;
            } // End SWITCH

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

void GetNextValueMemIntoToken
    (APLUINT  uArg,                         // Index to use
     LPVOID   lpMemArg,                     // Ptr to global memory object to index
     APLSTYPE aplTypeArg,                   // Storage type of the arg
     APLINT   apaOff,                       // APA offset (if needed)
     APLINT   apaMul,                       // APA multiplier (if needed)
     LPTOKEN  lptkArg)                      // Ptr to token in which to place the value

{
    // Clear the NoDisplay flag
    lptkArg->tkFlags.NoDisplay = FALSE;

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

        defstop
            break;
    } // End SWITCH
} // End GetNextValueMemIntoToken


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
            // ***FIXME*** -- Possible loss of precision
            return (APLFLOAT) ((LPAPLINT) lpMem)[uRes];

        case ARRAY_FLOAT:
            return ((LPAPLFLOAT) lpMem)[uRes];

        case ARRAY_APA:
            // ***FIXME*** -- Possible loss of precision
            return (APLFLOAT) (((LPAPLAPA) lpMem)->Off + ((LPAPLAPA) lpMem)->Mul * uRes);

        defstop
            return 0;
    } // End SWITCH
} // End GetNextFloat


//***************************************************************************
//  $GetNextHetero
//
//  Return the next value as Boolean/Integer, Float, or Char
//***************************************************************************

APLSTYPE GetNextHetero
    (LPAPLHETERO lpMem,                     // Ptr to global memory
     APLINT      uRes,                      // Index
     LPAPLINT    lpaplInteger,              // Ptr to Boolean/Integer result
     LPAPLFLOAT  lpaplFloat,                // Ptr to Float result
     LPAPLCHAR   lpaplChar)                 // Ptr to Char result

{
    LPSYMENTRY lpSymEntry;

    // Get the next LPSYMENTRY
    lpSymEntry = lpMem[uRes];

    // stData is immediate
    Assert (lpSymEntry->stFlags.Imm);

    // Split cases based upon the immediate storage type
    switch (lpSymEntry->stFlags.ImmType)
    {
        case IMMTYPE_BOOL:
            *lpaplInteger = lpSymEntry->stData.stBoolean;
            *lpaplFloat   = (APLFLOAT) *lpaplInteger;  // ***FIXME*** -- Possible loss of precision

            return ARRAY_BOOL;

        case IMMTYPE_INT:
            *lpaplInteger = lpSymEntry->stData.stInteger;
            *lpaplFloat   = (APLFLOAT) *lpaplInteger;  // ***FIXME*** -- Possible loss of precision

            return ARRAY_INT;

        case IMMTYPE_FLOAT:
            *lpaplFloat   = lpSymEntry->stData.stFloat;

            return ARRAY_FLOAT;

        case IMMTYPE_CHAR:
            *lpaplChar    = lpSymEntry->stData.stChar;

            return ARRAY_CHAR;

        defstop
            return ARRAY_ERROR;
    } // End SWITCH
} // End GetNextHetero


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
#ifdef DEBUG
    APLNELM   aplNELMSub;                   // Item NELM
#endif
    APLRANK   aplRankSub;                   // Item rank
    LPVOID    lpMemSub;                     // Ptr to item global memory

    // Lock the memory to get a ptr to it
    lpMemSub = MyGlobalLock (hGlbSub);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemSub)
    // Get the Array Type and Rank
    aplTypeSub = lpHeader->ArrType;
#ifdef DEBUG
    aplNELMSub = lpHeader->NELM;
#endif
    aplRankSub = lpHeader->Rank;
#undef  lpHeader

    Assert (uSub < aplNELMSub);

    // Skip over the header and dimensions to the data
    lpMemSub = VarArrayBaseToData (lpMemSub, aplRankSub);

    // Get next item from global memory
    GetNextItemMem (lpMemSub,               // Ptr to item global memory data
                    aplTypeSub,             // Item storage type
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
#ifdef DEBUG
    APLNELM   aplNELMSub;                   // Item NELM
#endif
    APLRANK   aplRankSub;                   // Item rank
    LPVOID    lpMemSub;                     // Ptr to item global memory

    // Clear the ptr type bits
    hGlbSub = ClrPtrTypeDir (hGlbSub);

    // Lock the memory to get a ptr to it
    lpMemSub = MyGlobalLock (hGlbSub);

    // Split cases based upon the signature
    switch (GetSignatureMem (lpMemSub))
    {
        case LSTARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPLSTARRAY_HEADER) lpMemSub)
            // Get the Array Type and Rank
            aplTypeSub = ARRAY_LIST;
#ifdef DEBUG
            aplNELMSub = lpHeader->NELM;
#endif
            aplRankSub = 1;
#undef  lpHeader
            // Skip over the header and dimensions to the data
            lpMemSub = LstArrayBaseToData (lpMemSub);

            break;

        case VARARRAY_HEADER_SIGNATURE:
#define lpHeader        ((LPVARARRAY_HEADER) lpMemSub)
            // Get the Array Type and Rank
            aplTypeSub = lpHeader->ArrType;
#ifdef DEBUG
            aplNELMSub = lpHeader->NELM;
#endif
            aplRankSub = lpHeader->Rank;
#undef  lpHeader
            // Skip over the header and dimensions to the data
            lpMemSub = VarArrayBaseToData (lpMemSub, aplRankSub);

            break;

        case DFN_HEADER_SIGNATURE:
        case FCNARRAY_HEADER_SIGNATURE:
        defstop
            break;
    } // End SWITCH

    Assert ((uSub < aplNELMSub)
         || ((uSub EQ 0) && IsNested (aplTypeSub)));

    // Get next value from global memory
    GetNextValueMem (lpMemSub,              // Ptr to item global memory data
                     aplTypeSub,            // Item storage type
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
     APLUINT     uSub,                      // Index into item
     HGLOBAL    *lphGlbRes,                 // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes,           // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

{
    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
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
     APLUINT     uSub,                  // Index into item
     HGLOBAL    *lphGlbRes,             // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes)       // Ptr to result immediate value (may be NULL)

{
    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
                        uSub,               // Index into item
                        lphGlbRes,          // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                        lpaplLongestRes,    // Ptr to result immediate value (may be NULL)
                        NULL);              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
} // End GetNextItemMem


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
     APLUINT     uSub,                  // Index into item
     HGLOBAL    *lphGlbRes,             // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
     APLLONGEST *lpaplLongestRes,       // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)          // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

{
    APLHETERO lpSymSub;                 // Item as APLHETERO
    LPAPLLIST lptkList;                 // Item as APLLIST

    // Assume the result is an immediate
    if (lphGlbRes)
        *lphGlbRes = NULL;

    // Split cases based upon the right arg storage type
    switch (aplTypeSub)
    {
        case ARRAY_BOOL:
            if (lpaplLongestRes)
                *lpaplLongestRes = BIT0 & (((LPAPLBOOL) lpMemSub)[uSub >> LOG2NBIB] >> (MASKLOG2NBIB & uSub));
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_BOOL;
            break;

        case ARRAY_INT:
            if (lpaplLongestRes)
                *lpaplLongestRes = ((LPAPLINT) lpMemSub)[uSub];
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_INT;
            break;

        case ARRAY_FLOAT:
            if (lpaplLongestRes)
                *lpaplLongestRes = *(LPAPLLONGEST) &((LPAPLFLOAT) lpMemSub)[uSub];
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_FLOAT;
            break;

        case ARRAY_CHAR:
            if (lpaplLongestRes)
                *lpaplLongestRes = ((LPAPLCHAR) lpMemSub)[uSub];
            if (lpimmTypeRes)
                *lpimmTypeRes    = IMMTYPE_CHAR;
            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemSub)
            if (lpaplLongestRes)
                *lpaplLongestRes = lpAPA->Off + lpAPA->Mul * uSub;
            if (lpimmTypeRes)
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
                    if (lpaplLongestRes)
                        *lpaplLongestRes = lptkList->tkData.tkLongest;
                    if (lpimmTypeRes)
                        *lpimmTypeRes    = lptkList->tkFlags.ImmType;
                    break;

                case TKT_VARARRAY:
                    if (lpaplLongestRes)
                        *lpaplLongestRes = 0;
                    if (lpimmTypeRes)
                        *lpimmTypeRes    = IMMTYPE_ERROR;
                    if (lphGlbRes)
                        *lphGlbRes       = ClrPtrTypeDir (lptkList->tkData.tkGlbData);
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
                    // If an immediate result is allowed, we never return the LPSYMENTRY
                    if (lphGlbRes && lpimmTypeRes EQ NULL)
                    {
                        *lphGlbRes = lpSymSub;
                        Assert (lpaplLongestRes EQ NULL);       // ***DEBUG***
#ifdef DEBUG
                        DbgBrk ();                              // ***DEBUG***
#endif
                    } // End IF

                    // Extract the immediate type & value
                    if (lpaplLongestRes)
                        *lpaplLongestRes = lpSymSub->stData.stLongest;
                    if (lpimmTypeRes)
                        *lpimmTypeRes    = lpSymSub->stFlags.ImmType;
                    break;

                case PTRTYPE_HGLOBAL:
                    if (lphGlbRes)
                        *lphGlbRes       = lpSymSub;

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH
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
    HGLOBAL hGlb;

    // Get the global handle w/o locking it
    GetGlbPtrs_LOCK (lpToken, &hGlb, NULL);

    return hGlb;
} // End GetGlbHandle


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
     LPVOID  *lplpMem)          // Ptr to ptr to memory (may be NULL)

{
    LPVOID     lpMem;           // Ptr to locked memory header
    APLLONGEST aplLongest;      // First value in the data (unless empty)
    APLSTYPE   aplTypeMem;      // Memory storage type
    APLNELM    aplNELMMem;      // Memory NELM

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                *lphGlb = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir_PTB (*lphGlb));

                // Handle the HGLOBAL case
                *lphGlb = ClrPtrTypeDir (*lphGlb);

                // Lock the memory to get a ptr to it
                lpMem = MyGlobalLock (*lphGlb);

#define lpHeader        ((LPVARARRAY_HEADER) lpMem)
                // Get the type & NELM
                aplTypeMem = lpHeader->ArrType;
                aplNELMMem = lpHeader->NELM;
#undef  lpHeader
                break;      // Continue with common HGLOBAL code
            } // End IF

            // Handle the immediate case
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkSym->stData.stLongest;

        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                *lphGlb = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir_PTB (*lphGlb)
                     || IsGlbTypeDfnDir_PTB (*lphGlb));

                // Handle the HGLOBAL case
                *lphGlb = ClrPtrTypeDir (*lphGlb);

                // Lock the memory to get a ptr to it
                lpMem = MyGlobalLock (*lphGlb);

                // Get the pseudo-type & NELM
                aplTypeMem = ARRAY_LIST;
                aplNELMMem = 0;

                break;      // Continue with common HGLOBAL code
            } // End IF

            // Handle the immediate case
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkSym->stData.stLongest;

        case TKT_VARIMMED:
        case TKT_AXISIMMED:
        case TKT_LSTIMMED:
        case TKT_FCNIMMED:
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkLongest;

        case TKT_FCNARRAY:
            *lphGlb = lpToken->tkData.tkGlbData;

            // Handle the HGLOBAL case
            *lphGlb = ClrPtrTypeDir (*lphGlb);

            // Lock the memory to get a ptr to it
            lpMem = MyGlobalLock (*lphGlb);

            // Get the pseudo-type & NELM
            aplTypeMem = ARRAY_LIST;
            aplNELMMem = 0;

            break;      // Continue with common HGLOBAL code

        case TKT_NUMSTRAND:
        case TKT_VARARRAY:
        case TKT_AXISARRAY:
        case TKT_LSTARRAY:
            *lphGlb = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (*lphGlb));

            // Handle the HGLOBAL case
            *lphGlb = ClrPtrTypeDir (*lphGlb);

            // Lock the memory to get a ptr to it
            lpMem = MyGlobalLock (*lphGlb);

#define lpHeader        ((LPVARARRAY_HEADER) lpMem)
            // Get the type & NELM
            aplTypeMem = lpHeader->ArrType;
            aplNELMMem = lpHeader->NELM;
#undef  lpHeader
            break;      // Continue with common HGLOBAL code

        case TKT_LSTMULT:
            *lphGlb = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL list array
            Assert (IsGlbTypeLstDir_PTB (*lphGlb));

            // Handle the HGLOBAL case
            *lphGlb = ClrPtrTypeDir (*lphGlb);

            // Lock the memory to get a ptr to it
            lpMem = MyGlobalLock (*lphGlb);

#define lpHeader        ((LPLSTARRAY_HEADER) lpMem)
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
     && !IsList (aplTypeMem))
        GetFirstValueGlb (*lphGlb,          // The global memory handle
                          NULL,             // Ptr to integer (or Boolean) (may be NULL)
                          NULL,             // ...    float (may be NULL)
                          NULL,             // ...    char (may be NULL)
                         &aplLongest,       // ...    longest (may be NULL)
                          NULL,             // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                          NULL,             // ...    immediate type (see IMM_TYPES) (may be NULL)
                          NULL,             // ...    array type -- ARRAY_TYPES (may be NULL)
                          TRUE);            // TRUE iff we should expand LPSYMENTRY into immediate value
    else
        aplLongest = 0;

    // If the caller wants the locked memory ptr, ...
    if (lplpMem)
        // Lock the memory to get a ptr to it
        *lplpMem = lpMem;
    else
    {
        MyGlobalUnlock (*lphGlb); lpMem = NULL;
    } // End IF/ELSE

    return aplLongest;
} // End GetGlbPtrs_LOCK


//***************************************************************************
//  $GetSteZero
//
//  Return the LPSYMENTRY corresponding to the constant zero
//***************************************************************************

LPSYMENTRY GetSteZero
    (void)

{
    LPPERTABDATA  lpMemPTD;     // Ptr to PerTabData global memory handle

    // Ensure we are where we think we are
    Assert (TLSTYPE_PL EQ TlsGetValue (dwTlsType));

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the STE
    return lpMemPTD->steZero;
} // End GetSteZero


//***************************************************************************
//  $GetSteOne
//
//  Return the LPSYMENTRY corresponding to the constant one
//***************************************************************************

LPSYMENTRY GetSteOne
    (void)

{
    LPPERTABDATA  lpMemPTD;     // Ptr to PerTabData global memory handle

    // Ensure we are where we think we are
    Assert (TLSTYPE_PL EQ TlsGetValue (dwTlsType));

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the STE
    return lpMemPTD->steOne;
} // End GetSteOne


//***************************************************************************
//  $GetSteBlank
//
//  Return the LPSYMENTRY corresponding to the constant blank
//***************************************************************************

LPSYMENTRY GetSteBlank
    (void)

{
    LPPERTABDATA  lpMemPTD;     // Ptr to PerTabData global memory handle

    // Ensure we are where we think we are
    Assert (TLSTYPE_PL EQ TlsGetValue (dwTlsType));

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the STE
    return lpMemPTD->steBlank;
} // End GetSteBlank


//***************************************************************************
//  $GetQuadCT
//
//  Get the current value of []CT
//***************************************************************************

APLFLOAT GetQuadCT
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    return lpMemPTD->htsPTD.lpSymQuad[SYSVAR_CT]->stData.stFloat;
} // End GetQuadCT


//***************************************************************************
//  $GetQuadIO
//
//  Get the current value of []IO
//***************************************************************************

APLBOOL GetQuadIO
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    return lpMemPTD->htsPTD.lpSymQuad[SYSVAR_IO]->stData.stBoolean;
} // End GetQuadIO


//***************************************************************************
//  $SetQuadIO
//
//  Set the current value of []IO
//***************************************************************************

void SetQuadIO
    (APLBOOL bQuadIO)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    lpMemPTD->htsPTD.lpSymQuad[SYSVAR_IO]->stData.stBoolean = bQuadIO;
} // End SetQuadIO


//***************************************************************************
//  $GetQuadPP
//
//  Get the current value of []PP
//***************************************************************************

APLUINT GetQuadPP
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    return lpMemPTD->htsPTD.lpSymQuad[SYSVAR_PP]->stData.stInteger;
} // End GetQuadPP


//***************************************************************************
//  $GetQuadPW
//
//  Get the current value of []PW
//***************************************************************************

APLUINT GetQuadPW
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    return lpMemPTD->htsPTD.lpSymQuad[SYSVAR_PW]->stData.stInteger;
} // End GetQuadPW


//***************************************************************************
//  $GetQuadRL
//
//  Get the current value of []RL
//***************************************************************************

APLUINT GetQuadRL
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the current value
    return lpMemPTD->htsPTD.lpSymQuad[SYSVAR_RL]->stData.stInteger;
} // End GetQuadRL


//***************************************************************************
//  $SetQuadRL
//
//  Set the current value of []RL
//***************************************************************************

void SetQuadRL
    (APLUINT uQuadRL)           // []RL

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Set the new value
    lpMemPTD->htsPTD.lpSymQuad[SYSVAR_RL]->stData.stInteger = uQuadRL;
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
    lpPrimSpec->QuadRL = GetQuadRL ();
} // End SavePrimSpecRL


//***************************************************************************
//  $RestPrimSpecRL
//
//  Restore []RL from lpPrimSpec
//***************************************************************************

void RestPrimSpecRL
    (LPPRIMSPEC lpPrimSpec)

{
    // Restore it
    SetQuadRL (lpPrimSpec->QuadRL);
} // End RestPrimSpecRL


//***************************************************************************
//  $GetPrototypeFcnPtr
//
//  Return a ptr to the appropriate prototype function
//    corresponding to a give function token type.
//***************************************************************************

LPPRIMFNS GetPrototypeFcnPtr
    (LPPL_YYSTYPE lpYYFcnStr)

{
    HGLOBAL      hGlbFcn;                   // Function array global memory handle
    LPPL_YYSTYPE lpMemFcn;                  // Ptr to function array global memory
    LPPRIMFNS    lpPrimFns;                 // Ptr to result

    // Split cases based upon the token type of the function strand's first item
    switch (lpYYFcnStr->tkToken.tkFlags.TknType)
    {
        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
            // Get a ptr to the prototype function for the first symbol (a function or operator)
            return PrimProtoFnsTab[SymTrans (&lpYYFcnStr->tkToken)];

        case TKT_FCNARRAY:
            // Split cases based upon the function array signature
            switch (GetSignatureGlb (lpYYFcnStr->tkToken.tkData.tkGlbData))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Get the function global memory handle
                    hGlbFcn = ClrPtrTypeDir (lpYYFcnStr->tkToken.tkData.tkGlbData);

                    // Lock the memory to get a ptr to it
                    lpMemFcn = MyGlobalLock (hGlbFcn);

                    // Skip over the header to the data
                    lpMemFcn = FcnArrayBaseToData (lpMemFcn);

                    // Recurse to get the result
                    lpPrimFns = GetPrototypeFcnPtr (lpMemFcn);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

                    return lpPrimFns;

                case DFN_HEADER_SIGNATURE:
                    // Get a ptr to the prototype function for the user-defined function/operator
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
    (LPPL_YYSTYPE lpYYFcnStr)

{
    // Split cases based upon the token type of the function strand's first item
    switch (lpYYFcnStr->tkToken.tkFlags.TknType)
    {
        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
            return &PrimFlags[SymTrans (&lpYYFcnStr->tkToken)];

        case TKT_FCNARRAY:
            return NULL;

        defstop
            return NULL;
    } // End SWITCH
} // End GetPrimFlagsPtr


//***************************************************************************
//  $GetTokenLineHandle
//
//  Return the global memory handle of a given
//***************************************************************************

HGLOBAL GetTokenLineHandle
    (HGLOBAL hGlbDfnHdr,                // User-defined function/operator global memory handle
     APLINT  aplLineNum)                // Target line #

{
    LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator header
    LPFCNLINE    lpFcnLines;            // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL      hGlbTknLine = NULL;    // Target tokenized line global memory handle

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

    // Ensure the line # is within range
    if (aplLineNum < 1
     || aplLineNum > lpMemDfnHdr->numFcnLines)
        goto ERROR_EXIT;

    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

    // Get the starting line's token global memory handles
    hGlbTknLine = lpFcnLines[aplLineNum - 1].hGlbTknLine;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

    return hGlbTknLine;
} // End GetTokenLineHandle


//***************************************************************************
//  $GetImmedType
//
//  Get the immediate type from a token
//***************************************************************************

IMM_TYPES GetImmedType
    (LPTOKEN lptkArg)               // Ptr to token

{
    if (IsTknTypeNamed (lptkArg->tkFlags.TknType))
        return lptkArg->tkData.tkSym->stFlags.ImmType;
    else
        return lptkArg->tkFlags.ImmType;
} // End GetImmedType


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

    // Clear the type bits in case they are set on the way in
    hGlbLcl = ClrPtrTypeDir (hGlbLcl);

    // Lock the memory to get a ptr to it
    lpMemLcl = MyGlobalLock (hGlbLcl);

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

    // Split cases based upon the ptr bits
    switch (GetPtrTypeDir (lpSymGlbLcl))
    {
        case PTRTYPE_STCONST:
            Assert (((LPSYMENTRY) lpSymGlbLcl)->stFlags.Imm EQ FALSE);

            // Clear the ptr type bits
            hGlbLcl = ClrPtrTypeDir (((LPSYMENTRY) lpSymGlbLcl)->stData.stGlbData);

            break;

        case PTRTYPE_HGLOBAL:
            // Clear the ptr type bits
            hGlbLcl = ClrPtrTypeDir (lpSymGlbLcl);

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
