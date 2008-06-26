//***************************************************************************
//  NARS2000 -- Getxxx Functions
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
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


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
                Assert (IsGlbTypeVarDir (hGlbData));

                break;      // Continue below with global case
            } // End IF

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                              lpToken->tkData.tkSym->stData.stLongest,
                              NULL,
                              NULL,
                              NULL,
                              lpaplLongest,
                              NULL,
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
                              NULL,
                              lpImmType,
                              NULL);
            return;

        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetFirstValueGlb (ClrPtrTypeDirAsGlb (hGlbData),    // The global memory handle
                      NULL,                             // Ptr to integer (or Boolean) (may be NULL)
                      NULL,                             // ...    float (may be NULL)
                      NULL,                             // ...    char (may be NULL)
                      lpaplLongest,                     // ...    longest (may be NULL)
                      lpSymGlb,                         // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                      lpImmType,                        // ...    immediate type (see IMM_TYPES) (may be NULL)
                      NULL,                             // ...    array type -- ARRAY_TYPES (may be NULL)
                      FALSE);                           // TRUE iff we should expand LPSYMENTRY into immediate value
} // End FirstItemToken


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
                Assert (IsGlbTypeVarDir (hGlbData));

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
                              lpSymGlb,
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
                              lpSymGlb,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetNextValueGlb (ClrPtrTypeDirAsGlb (hGlbData),     // The global memory handle
                     uIndex,                            // Index into item
                    &hGlbSub,                           // Ptr to result global memory handle (may be NULL)
                    &aplLongest,                        // Ptr to result immediate value (may be NULL)
                    &immType);                          // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // Fill in various result values
    if (lpaplLongest)
       *lpaplLongest = aplLongest;
    if (lpSymGlb)
       *lpSymGlb = hGlbSub;
    if (lpImmType)
       *lpImmType = immType;
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
                Assert (IsGlbTypeVarDir (hGlbData));

                break;      // Continue below with global case
            } // End IF

            // Handle the immediate case
            GetFirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                              lpToken->tkData.tkSym->stData.stLongest,
                              lpaplInteger,
                              lpaplFloat,
                              lpaplChar,
                              lpaplLongest,
                              lpSymGlb,
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
                              lpSymGlb,
                              lpImmType,
                              lpArrType);
            return;

        case TKT_VARARRAY:
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            break;      // Continue below with global case

        defstop
            return;
    } // End SWITCH

    // Handle the HGLOBAL case
    GetFirstValueGlb (ClrPtrTypeDirAsGlb (hGlbData),    // The global memory handle
                      lpaplInteger,                     // Ptr to integer (or Boolean) (may be NULL)
                      lpaplFloat,                       // ...    float (may be NULL)
                      lpaplChar,                        // ...    char (may be NULL)
                      lpaplLongest,                     // ...    longest (may be NULL)
                      lpSymGlb,                         // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                      lpImmType,                        // ...    immediate type (see IMM_TYPES) (may be NULL)
                      lpArrType,                        // ...    array type -- ARRAY_TYPES (may be NULL)
                      TRUE);                            // TRUE iff we should expand LPSYMENTRY into immediate value
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
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type (see IMM_TYPES) (may be NULL)
     LPAPLSTYPE   lpArrType)    // ...        array type:  ARRAY_TYPES (may be NULL)
{
    if (lpImmType)
        *lpImmType    = immType;
    if (lpArrType)
        *lpArrType    = TranslateImmTypeToArrayType (immType);
    if (lpSymGlb)
        *lpSymGlb     = NULL;
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
                *lpaplChar    = L'\0';
            break;

        case IMMTYPE_INT:
            if (lpaplInteger)
                *lpaplInteger = (APLINT) aplLongest;
            if (lpaplFloat)
                // ***FIXME*** -- Possible loss of precision
                *lpaplFloat   = (APLFLOAT) (APLINT) aplLongest;
            if (lpaplChar)
                *lpaplChar    = L'\0';
            break;

        case IMMTYPE_FLOAT:
            if (lpaplFloat)
                *lpaplFloat   = *(LPAPLFLOAT) &aplLongest;
            if (lpaplInteger)
                *lpaplInteger = (APLINT) *(LPAPLFLOAT) &aplLongest;
            if (lpaplChar)
                *lpaplChar    = L'\0';
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
     UBOOL        bExpandSym)       // TRUE iff we should expand LPSYMENTRY intoimmediate value

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

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the Array Type and NELM
    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;

    // Skip over the header and dimensions to the data
    lpMem = VarArrayBaseToData (lpMem, lpHeader->Rank);
#undef  lpHeader

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
                *lpaplChar    = L'\0';
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb = NULL;
            break;

        case ARRAY_INT:
            Assert (!IsEmpty (aplNELM));

            aplInteger = *(LPAPLINT) lpMem;

            if (lpaplInteger)
                *lpaplInteger = aplInteger;
            if (lpaplFloat)
                *lpaplFloat   = (APLFLOAT) aplInteger;  // ***FIXME*** -- Possible loss of precision
            if (lpaplChar)
                *lpaplChar    = L'\0';
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb = NULL;
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
                *lpaplChar    = L'\0';
            if (lpaplLongest)
                *lpaplLongest = aplInteger;
            if (lpSymGlb)
                *lpSymGlb = NULL;
            break;

        case ARRAY_FLOAT:
            Assert (!IsEmpty (aplNELM));

            aplFloat = *(LPAPLFLOAT) lpMem;

            if (lpaplFloat)
                *lpaplFloat   = aplFloat;
            if (lpaplInteger)
                *lpaplInteger = (APLINT) aplFloat;
            if (lpaplChar)
                *lpaplChar    = L'\0';
            if (lpaplLongest)
                *lpaplLongest = *(LPAPLLONGEST) &aplFloat;
            if (lpSymGlb)
                *lpSymGlb = NULL;
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
                *lpSymGlb = NULL;
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

                    // Handle the immediate case
                    GetFirstValueImm (lpSym->stFlags.ImmType,
                                      lpSym->stData.stLongest,
                                      lpaplInteger,
                                      lpaplFloat,
                                      lpaplChar,
                                      lpaplLongest,
                                      lpSymGlb,
                                      lpImmType,
                                      lpArrType);
                    break;

                case PTRTYPE_HGLOBAL:
                    if (lpaplInteger)
                        *lpaplInteger = 0;
                    if (lpaplFloat)
                        *lpaplFloat   = 0;
                    if (lpaplChar)
                        *lpaplChar    = L'\0';
                    if (lpaplLongest)
                        *lpaplLongest = 0;
                    if (lpSymGlb)
                        *lpSymGlb = *(LPAPLNESTED) lpMem;
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
//  $GetValueIntoToken
//
//  Get the next value from a variable into a token
//***************************************************************************

void GetValueIntoToken
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
                    lptkArg->tkFlags.ImmType  = 0;
                    lptkArg->tkData.tkGlbData = CopySymGlbDir (((LPAPLNESTED) lpMemArg)[uArg]);

                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH
} // End GetValueIntoToken


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
     HGLOBAL    *lphGlbRes,                 // Ptr to result global memory handle (may be NULL)
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
                    lphGlbRes,              // Ptr to result global memory handle (may be NULL)
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
     HGLOBAL    *lphGlbRes,                 // Ptr to result global memory handle (may be NULL)
     APLLONGEST *lpaplLongestRes,           // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

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

    Assert ((uSub < aplNELMSub)
         || ((uSub EQ 0) && IsNested (aplTypeSub)));

    // Skip over the header and dimensions to the data
    lpMemSub = VarArrayBaseToData (lpMemSub, aplRankSub);

    // Get next value from global memory
    GetNextValueMem (lpMemSub,              // Ptr to item global memory data
                     aplTypeSub,            // Item storage type
                     uSub,                  // Index into item
                     lphGlbRes,             // Ptr to result global memory handle (may be NULL)
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
     HGLOBAL    *lphGlbRes,                 // Ptr to result global memory handle (may be NULL)
     APLLONGEST *lpaplLongestRes,           // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)              // Ptr to result immediate type (see IMM_TYPES) (may be NULL)

{
    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
                        uSub,               // Index into item
                        lphGlbRes,          // Ptr to result global memory handle (may be NULL)
                        lpaplLongestRes,    // Ptr to result immediate value (may be NULL)
                        lpimmTypeRes,       // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        TRUE,               // TRUE iff we should expand LPSYMENTRY into immediate value
                        FALSE);             // TRUE if we're returning item (not value)
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
     HGLOBAL    *lphGlbRes,             // Ptr to result global memory handle (may be NULL)
     APLLONGEST *lpaplLongestRes)       // Ptr to result immediate value (may be NULL)

{
    GetNextValueMemSub (lpMemSub,           // Ptr to item global memory data
                        aplTypeSub,         // Item storage type
                        uSub,               // Index into item
                        lphGlbRes,          // Ptr to result global memory handle (may be NULL)
                        lpaplLongestRes,    // Ptr to result immediate value (may be NULL)
                        NULL,               // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        FALSE,              // TRUE iff we should expand LPSYMENTRY into immediate value
                        TRUE);              // TRUE if we're returning item (not value)
} // End GetNextItemMem


//***************************************************************************
//  $GetNextValueMemSub
//
//  Get next value from global memory
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextValueMemSub
    (LPVOID      lpMemSub,              // Ptr to item global memory data
     APLSTYPE    aplTypeSub,            // Item storage type
     APLUINT     uSub,                  // Index into item
     HGLOBAL    *lphGlbRes,             // Ptr to result global memory handle (may be NULL)
     APLLONGEST *lpaplLongestRes,       // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes,          // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
     UBOOL       bExpandSym,            // TRUE iff we should expand LPSYMENTRY intoimmediate value
     UBOOL       bGetItem)              // TRUE if we're returning item (not value)

{
    APLHETERO lpSymSub;                 // Item as APLHETERO

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

        case ARRAY_HETERO:
        case ARRAY_NESTED:
        case ARRAY_LIST:
            // Get the item
            lpSymSub = ((LPAPLNESTED) lpMemSub)[uSub];

            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (lpSymSub))
            {
                case PTRTYPE_STCONST:
                    if (!bExpandSym
                     && lphGlbRes)
                    {
                        *lphGlbRes = lpSymSub;

                        break;
                    } // End IF

                    // Extract the immediate type & value
                    if (lpaplLongestRes)
                        *lpaplLongestRes = lpSymSub->stData.stLongest;
                    if (lpimmTypeRes)
                        *lpimmTypeRes    = lpSymSub->stFlags.ImmType;
                    break;

                case PTRTYPE_HGLOBAL:
                    if (lphGlbRes)
                    {
                        if (bGetItem)
                            *lphGlbRes = lpSymSub;
                        else
                            *lphGlbRes = ClrPtrTypeDirAsGlb (lpSymSub);
                    } // End IF

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
    LPVARARRAY_HEADER lpMem;        // Ptr to locked memory header
    APLLONGEST        aplLongest;   // First value in the data (unless empty)

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
                Assert (IsGlbTypeVarDir (*lphGlb));

                break;      // Continue with common HGLOBAL code
            } // End IF

            // Handle the immediate case
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkSym->stData.stLongest;

        case TKT_VARIMMED:
        case TKT_AXISIMMED:
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkLongest;

        case TKT_VARARRAY:
        case TKT_AXISARRAY:
        case TKT_LISTBR:
            *lphGlb = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (*lphGlb));

            break;      // Continue with common HGLOBAL code

        defstop
            break;
    } // End SWITCH

    // Handle the HGLOBAL case
    *lphGlb = ClrPtrTypeDirAsGlb (*lphGlb);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (*lphGlb);

    // If the arg is non-empty, ...
    if (!IsEmpty (lpMem->NELM))
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
    LPSYMENTRY    lpSym;        // Ptr to result
    HGLOBAL       hGlbPTD;      // PerTabData global memory handle
    LPPERTABDATA  lpMemPTD;     // Ptr to PerTabData global memory handle

    // Ensure we are where we think we are
    Assert ('PL' EQ (__int3264) (HANDLE_PTR) TlsGetValue (dwTlsType));

    // Get the PerTabData global handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the STE
    lpSym = lpMemPTD->steZero;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpSym;
} // End GetSteZero


//***************************************************************************
//  $GetSteOne
//
//  Return the LPSYMENTRY corresponding to the constant one
//***************************************************************************

LPSYMENTRY GetSteOne
    (void)

{
    LPSYMENTRY    lpSym;        // Ptr to result
    HGLOBAL       hGlbPTD;      // PerTabData global memory handle
    LPPERTABDATA  lpMemPTD;     // Ptr to PerTabData global memory handle

    // Ensure we are where we think we are
    Assert ('PL' EQ (__int3264) (HANDLE_PTR) TlsGetValue (dwTlsType));

    // Get the PerTabData global handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the STE
    lpSym = lpMemPTD->steOne;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpSym;
} // End GetSteOne


//***************************************************************************
//  $GetSteBlank
//
//  Return the LPSYMENTRY corresponding to the constant blank
//***************************************************************************

LPSYMENTRY GetSteBlank
    (void)

{
    LPSYMENTRY    lpSym;        // Ptr to result
    HGLOBAL       hGlbPTD;      // PerTabData global memory handle
    LPPERTABDATA  lpMemPTD;     // Ptr to PerTabData global memory handle

    // Ensure we are where we think we are
    Assert ('PL' EQ (__int3264) (HANDLE_PTR) TlsGetValue (dwTlsType));

    // Get the PerTabData global handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the STE
    lpSym = lpMemPTD->steBlank;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpSym;
} // End GetSteBlank


//***************************************************************************
//  $GetQuadCT
//
//  Get the current value of []CT
//***************************************************************************

APLFLOAT GetQuadCT
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLFLOAT     fQuadCT;       // []CT

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    fQuadCT = lpMemPTD->lpSymQuadCT->stData.stFloat;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return fQuadCT;
} // End GetQuadCT


//***************************************************************************
//  $GetQuadIO
//
//  Get the current value of []IO
//***************************************************************************

APLBOOL GetQuadIO
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLBOOL      bQuadIO;       // []IO

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    bQuadIO = lpMemPTD->lpSymQuadIO->stData.stBoolean;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bQuadIO;
} // End GetQuadIO


//***************************************************************************
//  $GetQuadPP
//
//  Get the current value of []PP
//***************************************************************************

APLUINT GetQuadPP
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLUINT      uQuadPP;       // []PP

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    uQuadPP = lpMemPTD->lpSymQuadPP->stData.stInteger;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return uQuadPP;
} // End GetQuadPP


//***************************************************************************
//  $GetQuadPW
//
//  Get the current value of []PW
//***************************************************************************

APLUINT GetQuadPW
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLUINT      uQuadPW;       // []PW

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    uQuadPW = lpMemPTD->lpSymQuadPW->stData.stInteger;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return uQuadPW;
} // End GetQuadPW


//***************************************************************************
//  $GetQuadRL
//
//  Get the current value of []RL
//***************************************************************************

APLUINT GetQuadRL
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    APLUINT      uQuadRL;       // []RL

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the current value
    uQuadRL = lpMemPTD->lpSymQuadRL->stData.stInteger;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return uQuadRL;
} // End GetQuadRL


//***************************************************************************
//  $SetQuadRL
//
//  Set the current value of []RL
//***************************************************************************

void SetQuadRL
    (APLUINT uQuadRL)           // []RL

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set the new value
    lpMemPTD->lpSymQuadRL->stData.stInteger = uQuadRL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
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
            // Get a ptr to the prototype function for the user-defined function/operator
            return &ExecDfnGlbProto_EM_YY;

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
//  End of File: getfns.c
//***************************************************************************
