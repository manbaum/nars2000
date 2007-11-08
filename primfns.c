//***************************************************************************
//  NARS2000 -- Primitive Functions
//***************************************************************************

#define STRICT
#include <windows.h>
#include <float.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $IsFcnOpr
//
//  Determine if a given token is a function or operator
//***************************************************************************

BOOL IsFcnOpr
    (LPTOKEN lptk)

{
    switch (TokenTypeFV (lptk))
    {
        case 'F':
        case '1':
        case '2':
        case '3':
            return TRUE;

        case 'V':
            return FALSE;

        defstop
            return 0;
    } // End SWITCH
} // End IsFcnOpr


//***************************************************************************
//  $TokenTypeFV
//
//  Return the type of a token
//
//  'F' = Function
//  'V' = Variable
//  '1' = Monadic operator
//  '2' = Dyadic operator
//  '3' = Ambiguous operator
//  '?' = None of the above
//
//***************************************************************************

char TokenTypeFV
    (LPTOKEN lptk)

{
    // Split cases based upon the token type
    switch (lptk->tkFlags.TknType)
    {
        case TKT_VARNAMED:
        case TKT_STRING:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            return 'V';

        case TKT_FCNIMMED:
        case TKT_FCNARRAY:
        case TKT_FCNNAMED:
            return 'F';

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OPJOTDOT:
            return '1';

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
            return '2';

        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
            return '3';

        case TKT_COMMENT:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_LABELSEP:
        case TKT_COLON:
        case TKT_LPAREN:
        case TKT_RPAREN:
        case TKT_LBRACKET:
        case TKT_RBRACKET:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_SOS:
        case TKT_LINECONT:
        case TKT_INPOUT:
        case TKT_STRAND:
        case TKT_LISTINT:
        case TKT_LISTPAR:
        case TKT_LISTBR:
        case TKT_AXISIMMED:
        case TKT_AXISARRAY:
        case TKT_STRNAMED:
        defstop
            return '?';
    } // End SWITCH
} // End TokenTypeFV


//***************************************************************************
//  $RankOfGlb
//
//  Get the rank of a global memory object
//***************************************************************************

APLRANK RankOfGlb
    (HGLOBAL hGlb)

{
    LPVOID  lpMem;
    APLRANK aplRank;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlb);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    // Get the Rank
    aplRank = lpHeader->Rank;
#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMem = NULL;

    return aplRank;
} // End RankOfGlb


//***************************************************************************
//  $AttrsOfToken
//
//  Return the attributes (Type, NELM, and Rank) of a token.
//***************************************************************************

void AttrsOfToken
    (LPTOKEN   lpToken,         // Ptr to token
     APLSTYPE *lpaplType,       // Ptr to token storage type (may be NULL)
     LPAPLNELM lpaplNELM,       // ...          NELM (may be NULL)
     LPAPLRANK lpaplRank,       // ...          rank (may be NULL)
     LPAPLDIM  lpaplCols)       // ...          # cols in the array (scalar = 1) (may be NULL)

{
    HGLOBAL hGlbData;           // Global memory handle

    // Split case based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must traverse the array
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Get the global handle
                hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbData));

                break;      // Continue with HGLOBAL case
            } // End IF

            if (lpaplType)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkData.tkSym->stFlags.ImmType);
            if (lpaplNELM)
                *lpaplNELM = 1;
            if (lpaplRank)
                *lpaplRank = 0;
            if (lpaplCols)
                *lpaplCols = 1;
            return;

        case TKT_VARIMMED:
            if (lpaplType)
                *lpaplType = TranslateImmTypeToArrayType (lpToken->tkFlags.ImmType);
            if (lpaplNELM)
                *lpaplNELM = 1;
            if (lpaplRank)
                *lpaplRank = 0;
            if (lpaplCols)
                *lpaplCols = 1;
            return;

        case TKT_VARARRAY:
            // Get the global handle
            hGlbData = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbData));

            break;      // Continue with HGLOBAL case

        case TKT_LISTPAR:
        case TKT_LISTBR:
            hGlbData = lpToken->tkData.tkGlbData;

            break;      // Continue with HGLOBAL case

        defstop
            return;
    } // End SWITCH

    // Get the attributes (Type, NELM, and Rank) of the global
    AttrsOfGlb (ClrPtrTypeDirGlb (hGlbData), lpaplType, lpaplNELM, lpaplRank, lpaplCols);
} // End AttrsOfToken


//***************************************************************************
//  $AttrsOfGlb
//
//  Return the attributes (Type, NELM, and Rank) of a global memory handle.
//***************************************************************************

void AttrsOfGlb
    (HGLOBAL   hGlbData,        // Memory handle
     APLSTYPE *lpaplType,       // Ptr to token storage type (may be NULL)
     LPAPLNELM lpaplNELM,       // ...          NELM (may be NULL)
     LPAPLRANK lpaplRank,       // ...          rank (may be NULL)
     LPAPLUINT lpaplCols)       // ...          # cols in the array (scalar = 1) (may be NULL)

{
    LPVOID  lpMemData;          // Ptr to global memory

    // Lock the memory to get a ptr to it
    lpMemData = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemData)
    if (lpaplType)
        *lpaplType = lpHeader->ArrType;
    if (lpaplNELM)
        *lpaplNELM = lpHeader->NELM;
    if (lpaplRank)
        *lpaplRank = lpHeader->Rank;

    // Skip over the header to the dimensions
    if (lpaplCols)
    {
        if (lpHeader->Rank NE 0)
            *lpaplCols = ((LPAPLDIM) VarArrayBaseToDim (lpMemData))[lpHeader->Rank - 1];
        else
            *lpaplCols = 1;
    } // End IF
#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemData = NULL;
} // End AttrsOfGlb


//***************************************************************************
//  $StorageType
//
//  Return the storage type for a dyadic scalar function
//    given the left and right storage types and the function.
//***************************************************************************

APLSTYPE StorageType
    (APLSTYPE aplTypeLft,   // Storage type of left arg
     LPTOKEN  lptkFunc,     // Immediate function
     APLSTYPE aplTypeRht)   // Storage type of right arg

{
    static APLSTYPE aplTypeArr[ARRAY_LENGTH][ARRAY_LENGTH] =
    //      BOOL          INT           FLOAT         CHAR        HETERO        NESTED        LIST         APA
    {{ARRAY_BOOL  , ARRAY_INT   , ARRAY_FLOAT , ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT   },  // BOOL
     {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT   },  // INT
     {ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_FLOAT , ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_FLOAT },  // FLOAT
     {ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_ERROR , ARRAY_MIXED },  // CHAR
     {ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_MIXED , ARRAY_ERROR , ARRAY_MIXED },  // HETERO
     {ARRAY_NESTED, ARRAY_NESTED, ARRAY_NESTED, ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_NESTED},  // NESTED
     {ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR , ARRAY_ERROR },  // LIST
     {ARRAY_INT   , ARRAY_INT   , ARRAY_FLOAT , ARRAY_MIXED , ARRAY_MIXED , ARRAY_NESTED, ARRAY_ERROR , ARRAY_INT   },  // APA
    };

    APLSTYPE aplTypeRes;

    // Both arguments are within bounds
    Assert (aplTypeLft < ARRAY_LENGTH);
    Assert (aplTypeRht < ARRAY_LENGTH);

    // tkData is an immediate function
    Assert (lptkFunc->tkFlags.TknType EQ TKT_FCNIMMED);

    // Get the common storage type
    aplTypeRes = aplTypeArr[aplTypeLft][aplTypeRht];

    // Split cases based upon the result type
    switch (aplTypeRes)
    {
        case ARRAY_ERROR:
            return aplTypeRes;

        case ARRAY_MIXED:
            // A mixed result is from CHAR or HETERO vs. numeric (BOOL, INT, APA, FLOAT) or NESTED
            // The result is BOOL or NESTED if the function is equal or not-equal, otherwise
            //   the result is ERROR.
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
            {
                if (aplTypeLft EQ ARRAY_NESTED
                 || aplTypeRht EQ ARRAY_NESTED)
                    return ARRAY_NESTED;
                else
                    return ARRAY_BOOL;
            } else
                return ARRAY_ERROR;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
            if (lptkFunc->tkData.tkChar EQ UTF16_EQUAL
             || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL)
                return ARRAY_BOOL;
            else
                return aplTypeRes;

        case ARRAY_NESTED:
            return aplTypeRes;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_APA:
        defstop
            return ARRAY_ERROR;
    } // End SWITCH
} // StorageType


//***************************************************************************
//  $FirstItem
//
//  Return the first item from the token
//***************************************************************************

void FirstItem
    (LPTOKEN      lpToken,      // Ptr to the token
     LPAPLLONGEST lpaplLongest, // Ptr to the longest (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type:  IMM_TYPES (may be NULL)
     LPVOID      *lpSymGlb)     // ...        LPSYMENTRY or HGLOBAL (may be NULL)

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
            FirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
                           lpToken->tkData.tkSym->stData.stLongest,
                           NULL,
                           NULL,
                           NULL,
                           lpaplLongest,
                           NULL,
                           NULL,
                           NULL);
            return;

        case TKT_VARIMMED:
            // Handle the immediate case
            FirstValueImm (lpToken->tkFlags.ImmType,
                           lpToken->tkData.tkLongest,
                           NULL,
                           NULL,
                           NULL,
                           lpaplLongest,
                           NULL,
                           NULL,
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
    FirstValueGlb (ClrPtrTypeDirGlb (hGlbData), // The global memory handle
                   NULL,                        // Ptr to integer (or Boolean) (may be NULL)
                   NULL,                        // ...    float (may be NULL)
                   NULL,                        // ...    char (may be NULL)
                   lpaplLongest,                // ...    longest (may be NULL)
                   lpSymGlb,                    // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                   lpImmType,                   // ...    immediate type IMM_TYPES (may be NULL)
                   NULL,                        // ...    array type -- ARRAY_TYPES (may be NULL)
                   FALSE);                      // TRUE iff we should recurse through LPSYMENTRYs
} // End FirstItem


//***************************************************************************
//  $FirstValue
//
//  Return the first value from a token as either
//    both an integer and a float, or as a character,
//    or as an LPSYMENTRY/HGLOBAL.  The token may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void FirstValue
    (LPTOKEN      lpToken,      // Ptr to the token
     LPAPLINT     lpaplInteger, // Ptr to the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type:  IMM_TYPES (may be NULL)
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
            FirstValueImm (lpToken->tkData.tkSym->stFlags.ImmType,
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
            FirstValueImm (lpToken->tkFlags.ImmType,
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
    FirstValueGlb (ClrPtrTypeDirGlb (hGlbData), // The global memory handle
                   lpaplInteger,                // Ptr to integer (or Boolean) (may be NULL)
                   lpaplFloat,                  // ...    float (may be NULL)
                   lpaplChar,                   // ...    char (may be NULL)
                   lpaplLongest,                // ...    longest (may be NULL)
                   lpSymGlb,                    // ...    LPSYMENTRY or HGLOBAL (may be NULL)
                   lpImmType,                   // ...    immediate type IMM_TYPES (may be NULL)
                   lpArrType,                   // ...    array type -- ARRAY_TYPES (may be NULL)
                   TRUE);                       // TRUE iff we should recurse through LPSYMENTRYs
} // End FirstValue


//***************************************************************************
//  $FirstValueImm
//
//  Return the first value of an immediate
//***************************************************************************

void FirstValueImm
    (IMM_TYPES    immType,      // The token's immediate type
     APLLONGEST   aplLongest,   // The longest immediate value
     LPAPLINT     lpaplInteger, // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type:  IMM_TYPES (may be NULL)
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
} // End FirstValueImm


//***************************************************************************
//  $FirstValueGlb
//
//  Return the first value from an HGLOBAL as either
//    both an integer and a float, or as a character,
//    or as an HGLOBAL.  The HGLOBAL may be
//    an empty array in which case the value of the
//    prototype is returned.
//***************************************************************************

void FirstValueGlb
    (HGLOBAL      hGlbData,     // The global memory handle
     LPAPLINT     lpaplInteger, // Return the integer (or Boolean) (may be NULL)
     LPAPLFLOAT   lpaplFloat,   // ...        float (may be NULL)
     LPAPLCHAR    lpaplChar,    // ...        char (may be NULL)
     LPAPLLONGEST lpaplLongest, // ...        longest (may be NULL)
     LPVOID      *lpSymGlb,     // ...        LPSYMENTRY or HGLOBAL (may be NULL)
     LPIMM_TYPES  lpImmType,    // ...        immediate type IMM_TYPES (may be NULL)
     LPAPLSTYPE   lpArrType,    // ...        array type -- ARRAY_TYPES (may be NULL)
     BOOL         bRecurse)     // TRUE iff we should recurse through LPSYMENTRYs

{
    LPVOID     lpMem;
    APLSTYPE   aplType;
    APLNELM    aplNELM;
    APLINT     aplInteger;
    APLFLOAT   aplFloat;
    APLCHAR    aplChar;
    LPSYMENTRY lpSym;

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
            Assert (aplNELM > 0);

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
            Assert (aplNELM > 0);

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

        case ARRAY_NESTED:
            // If it's an HGLOBAL, ...
            if (GetPtrTypeInd (lpMem) EQ PTRTYPE_HGLOBAL)
            {
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
            } // End IF

            // Fall through to ARRAY_HETERO case

        case ARRAY_HETERO:
            if (!bRecurse)
            {
                if (lpSymGlb)
                {
                    *lpSymGlb = *(LPAPLNESTED) lpMem;

                    break;
                } // End IF
            } // End IF

            Assert ((aplType EQ ARRAY_NESTED) || aplNELM > 0);

            lpSym = *(LPAPLHETERO) lpMem;

            // stData is immediate
            Assert (lpSym->stFlags.Imm);

            // Handle the immediate case
            FirstValueImm (lpSym->stFlags.ImmType,
                           lpSym->stData.stLongest,
                           lpaplInteger,
                           lpaplFloat,
                           lpaplChar,
                           lpaplLongest,
                           lpSymGlb,
                           lpImmType,
                           lpArrType);
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End FirstValueGlb


//***************************************************************************
//  $GetValueIntoToken
//
//  Get the next value from a variable into a token
//***************************************************************************

void GetValueIntoToken
    (APLUINT  uArg,         // Index to use
     LPVOID   lpMemArg,     // Ptr to global memory object to index
     APLSTYPE aplTypeArg,   // Storage type of the arg
     APLINT   apaOff,       // APA offset (if needed)
     APLINT   apaMul,       // APA multiplier (if needed)
     LPTOKEN  lptkArg)      // Ptr to token in which to place the value

{
    // Clear the NoDisplay flag
    lptkArg->tkFlags.NoDisplay = 0;

    // Split cases based upon the arg storage type
    switch (aplTypeArg)
    {
        case ARRAY_BOOL:
            lptkArg->tkFlags.TknType  = TKT_VARIMMED;
            lptkArg->tkFlags.ImmType  = IMMTYPE_BOOL;
            lptkArg->tkData.tkInteger = BIT0 & (UCHAR) (((LPAPLBOOL) lpMemArg)[uArg >> LOG2NBIB] >> (uArg & MASKLOG2NBIB));

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
//  $GetGlbPtrs_LOCK
//
//  Return the HGLOBAL and LPVOID from a token if it's an HGLOBAL
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
            *lphGlb  = NULL;

            if (lplpMem)
                *lplpMem = NULL;

            return lpToken->tkData.tkLongest;

        case TKT_VARARRAY:
        case TKT_LISTBR:
            *lphGlb = lpToken->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (*lphGlb));

            break;      // Continue with common HGLOBAL code

        defstop
            break;
    } // End SWITCH

    // Handle the HGLOBAL case
    if (*lphGlb)
    {
        *lphGlb = ClrPtrTypeDirGlb (*lphGlb);

        // Lock the memory to get a ptr to it
        if (lplpMem)
            *lplpMem = MyGlobalLock (*lphGlb);
    } // End IF

    return (APLLONGEST) 0;
} // End GetGlbPtrs_LOCK


//***************************************************************************
//  $GetNextInteger
//
//  Return the next value as an integer
//***************************************************************************

APLINT GetNextInteger
    (LPVOID   lpMem,                // Ptr to global memory
     APLSTYPE aplType,              // Storage type
     APLINT   uRes)                 // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return BIT0 & (((LPAPLBOOL) lpMem)[uRes >> LOG2NBIB] >> (uRes & MASKLOG2NBIB));

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
    (LPVOID   lpMem,                // Ptr to global memory
     APLSTYPE aplType,              // Storage type
     APLINT   uRes)                 // Index

{
    // Split cases based upon the storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            return (APLFLOAT) (BIT0 & (((LPAPLBOOL) lpMem)[uRes >> LOG2NBIB] >> (uRes & MASKLOG2NBIB)));

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
    (LPAPLHETERO lpMem,             // Ptr to global memory
     APLINT      uRes,              // Index
     LPAPLINT    lpaplInteger,      // Ptr to Boolean/Integer result
     LPAPLFLOAT  lpaplFloat,        // Ptr to Float result
     LPAPLCHAR   lpaplChar)         // Ptr to Char result

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
//  $GetNextValueGlb
//
//  Get next value from a global memory handle
//  If *lphGlbRes is NULL on exit, the result is an immediate.
//***************************************************************************

void GetNextValueGlb
    (HGLOBAL     hGlbSub,               // Item global memory handle
     APLUINT     uSub,                  // Index into item
     HGLOBAL    *lphGlbRes,             // Ptr to result global memory handle (may be NULL)
     APLLONGEST *lpaplLongestRes,       // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)          // Ptr to result immediate type (may be NULL)

{
    APLSTYPE  aplTypeSub;               // Item storage type
#ifdef DEBUG
    APLNELM   aplNELMSub;               // Item NELM
#endif
    APLRANK   aplRankSub;               // Item rank
    LPVOID    lpMemSub;                 // Ptr to item global memory

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
    (LPVOID      lpMemSub,              // Ptr to item global memory data
     APLSTYPE    aplTypeSub,            // Item storage type
     APLUINT     uSub,                  // Index into item
     HGLOBAL    *lphGlbRes,             // Ptr to result global memory handle (may be NULL)
     APLLONGEST *lpaplLongestRes,       // Ptr to result immediate value (may be NULL)
     IMM_TYPES  *lpimmTypeRes)          // Ptr to result immediate type (may be NULL)

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
                *lpaplLongestRes = BIT0 & (((LPAPLBOOL) lpMemSub)[uSub >> LOG2NBIB] >> (uSub & MASKLOG2NBIB));
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
            lpSymSub = ((LPAPLHETERO) lpMemSub)[uSub];

            // Extract the immediate type & value
            if (lpaplLongestRes)
                *lpaplLongestRes = lpSymSub->stData.stLongest;
            if (lpimmTypeRes)
                *lpimmTypeRes    = lpSymSub->stFlags.ImmType;
            break;

        case ARRAY_NESTED:
        case ARRAY_LIST:
            // Get the item
            lpSymSub = ((LPAPLNESTED) lpMemSub)[uSub];

            // Split cases based upon the ptr type
            switch (GetPtrTypeDir (lpSymSub))
            {
                case PTRTYPE_STCONST:
                    // Extract the immediate type & value
                    if (lpaplLongestRes)
                        *lpaplLongestRes = lpSymSub->stData.stLongest;
                    if (lpimmTypeRes)
                        *lpimmTypeRes    = lpSymSub->stFlags.ImmType;
                    break;

                case PTRTYPE_HGLOBAL:
                    // Mark the result as an HGLOBAL
                    if (lphGlbRes)
                        *lphGlbRes = ClrPtrTypeDirGlb (lpSymSub);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        defstop
            break;
    } // End SWITCH
} // End GetNextValueMem


//***************************************************************************
//  $IncrOdometer
//
//  Increment the odometer in lpMemOdo subject to the
//    limits in lpMemDim[lpMemAxis] or lpMemDim, all
//    of length <aplRank>.
//***************************************************************************

void IncrOdometer
    (LPAPLUINT lpMemOdo,    // Ptr to the values to increment
     LPAPLDIM  lpMemDim,    // Ptr to the limits on each dimension
     LPAPLINT  lpMemAxis,   // Ptr to optional index vector (may be NULL)
     APLRANK   aplRank)     // # elements to which each of the
                            //   ptrs point

{
    APLINT iOdo;

    // Loop through the odometer values from right to left
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    if (lpMemAxis)
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] EQ lpMemDim[lpMemAxis[iOdo]])
            lpMemOdo[iOdo] = 0;
        else
            break;
    } else
    {
        for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
        if (++lpMemOdo[iOdo] EQ lpMemDim[iOdo])
            lpMemOdo[iOdo] = 0;
        else
            break;
    } // End IF/ELSE
} // End IncrOdometer


//***************************************************************************
//  $PrimScalarFnDydAllocate_EM
//
//  Allocate space for result of a primitive scalar dyadic function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimScalarFnDydAllocate_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimScalarFnDydAllocate_EM
    (LPTOKEN    lptkFunc,       // Ptr to function token

     HGLOBAL   *lphGlbRes,      // Return HGLOBAL of the result

     LPVOID     lpMemLft,       // Ptr to left arg memory (may be NULL if monadic)
     LPVOID     lpMemRht,       // ...    right ...

     APLRANK    aplRankLft,     // Left arg rank
     APLRANK    aplRankRht,     // Right ...
     LPAPLRANK  lpaplRankRes,   // Ptr to result rank

     APLSTYPE   aplTypeRes,     // Result type

     APLNELM    aplNELMLft,     // Left arg NELM
     APLNELM    aplNELMRht,     // Right ...
     APLNELM    aplNELMRes)     // Result ...

{
    APLUINT  ByteRes;           // # bytes in the result
    LPVOID   lpMemRes;          // Ptr to locked memory in result
    LPAPLDIM lpMemDimArg;
    APLUINT  uRes;
    LPVOID   lpMem;

    // Split cases based upon the singleton status
    //   of the left & right args
    if (aplNELMLft EQ 1
     && aplNELMRht EQ 1)
    {
        //***************************************************************
        // Both args are singletons
        //***************************************************************

        // The rank of the result is the larger of the two args
        *lpaplRankRes = max (aplRankLft, aplRankRht);

        // If it's a simple numeric scalar, it'll
        //   be stored as an immediate token,
        //   not global memory.
        if (*lpaplRankRes EQ 0
         && IsSimpleNum (aplTypeRes))
            return TRUE;
    } else
    if (aplNELMLft EQ 1
     || aplNELMRht EQ 1)
    {
        //***************************************************************
        // One of the args is a singleton, the other not
        //***************************************************************

        // The rank of the result is the rank of the non-singleton
        if (aplNELMLft NE 1)
            *lpaplRankRes = aplRankLft;
        else
            *lpaplRankRes = aplRankRht;
    } else
    {
        //***************************************************************
        // Neither arg is a singleton
        //***************************************************************

        // The rank of the result is the larger of the two args
        *lpaplRankRes = max (aplRankLft, aplRankRht);
    } // End IF/ELSE/...

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, *lpaplRankRes);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    *lphGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!*lphGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = *lpaplRankRes;
#undef  lpHeader

    // Fill in the dimensions
    if (aplNELMLft EQ 1
     && aplNELMRht EQ 1)
    {
        // Fill in the dimensions (all 1)
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = 1;
    } else
    if (aplNELMLft EQ 1
     || aplNELMRht EQ 1)
    {
        // Copy the ptr to the non-singleton argument
        if (aplNELMLft NE 1)
            lpMemDimArg = lpMemLft;
        else
            lpMemDimArg = lpMemRht;

        // Copy the dimensions from the non-singleton arg
        //   to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } else
    {
        // Get the higher rank's pointer
        //   to copy its dimensions to the result
        if (aplRankLft > aplRankRht)
            lpMemDimArg = lpMemLft;
        else
            lpMemDimArg = lpMemRht;

        // Copy the dimensions to the result
        for (uRes = 0; uRes < *lpaplRankRes; uRes++)
            (VarArrayBaseToDim (lpMemRes))[uRes] = (VarArrayBaseToDim (lpMemDimArg))[uRes];
    } // End IF/ELSE/...

    // Fill nested result with PTR_REUSED
    //   in case we fail part way through
    if (aplTypeRes EQ ARRAY_NESTED)
    {
        // Skip over the header and dimensions to the data
        lpMem = VarArrayBaseToData (lpMemRes, *lpaplRankRes);

        // Fill in the prototype
        *((LPAPLNESTED) lpMem) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMem)[uRes] = PTR_REUSED;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    return TRUE;
} // End PrimScalarFnDydAllocate_EM
#undef  APPEND_NAME


//***************************************************************************
//  $MakeMonPrototype_EM
//
//  Make a prototype copy of a single global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeMonPrototype_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL MakeMonPrototype_EM
    (HGLOBAL    hGlbArr,            // Incoming array handle
     LPTOKEN    lptkFunc,           // Ptr to function token
     MAKE_PROTO mpEnum)             // See MAKE_PROTO

{
    LPVOID      lpMemArr,
                lpMemRes;
    LPVARARRAY_HEADER lpHeader;
    APLSTYPE    aplType;
    APLNELM     aplNELM;
    APLRANK     aplRank;
    UINT        u;
    APLNELM     uLen;
    HGLOBAL     hGlbTmp;
    BOOL        bRet = TRUE;        // TRUE iff result is valid
    APLUINT     ByteRes;            // # bytes in the result
    LPSYMENTRY  lpSymArr,
                lpSymRes;

    DBGENTER;

    // Make a copy of the array as we're changing it
    hGlbArr = CopyArray_EM (hGlbArr,
                            lptkFunc);
    if (!hGlbArr)
        return NULL;

    // Lock the memory to get a ptr to it
    lpMemArr = lpHeader = MyGlobalLock (hGlbArr);

    aplType = lpHeader->ArrType;
    aplNELM = lpHeader->NELM;
    aplRank = lpHeader->Rank;

    // Point to the data
    lpMemArr = VarArrayBaseToData (lpMemArr, aplRank);

    // Split cases based upon the array type
    switch (aplType)
    {
        case ARRAY_BOOL:
            // Calculate # bytes of Boolean data
            uLen = RoundUpBits8 (aplNELM);

            for (u = 0; u < uLen; u++)
                *((LPAPLBOOL)  lpMemArr)++ = 0x00;
            break;

        case ARRAY_INT:
            for (u = 0; u < aplNELM; u++)
                *((LPAPLINT)   lpMemArr)++ = 0;
            break;

        case ARRAY_FLOAT:
            for (u = 0; u < aplNELM; u++)
                *((LPAPLFLOAT) lpMemArr)++ = 0;
            break;

        case ARRAY_CHAR:
            // Split cases based upon mpEnum
            switch (mpEnum)
            {
                case MP_CHARS:
                    break;

                case MP_NUMONLY:
                    // If the arg is non-empty, that's an error
                    if (aplNELM NE 0)
                        goto DOMAIN_ERROR_EXIT;

                    // Change the storage type to Boolean
                    lpHeader->ArrType = ARRAY_BOOL;

                    break;

                case MP_NUMCONV:
                    // Convert the chars to numerics by allocating
                    //   new (Boolean) storage and copying over
                    //   the dimensions

                    // Calculate space needed for the result
                    ByteRes = CalcArraySize (ARRAY_BOOL, aplNELM, aplRank);

                    // Allocate space for the result.
                    // N.B. Conversion from APLUINT to UINT.
                    Assert (ByteRes EQ (UINT) ByteRes);
                    hGlbTmp = DbgGlobalAlloc (GHND, (UINT) ByteRes);
                    if (!hGlbTmp)
                    {
                        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Lock the memory to get a ptr to it
                    lpMemRes = MyGlobalLock (hGlbTmp);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
                    // Fill in the header
                    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                    lpHeader->ArrType    = ARRAY_BOOL;
////////////////////lpHeader->Perm       = 0;   // Already zero from GHND
////////////////////lpHeader->SysVar     = 0;   // Already zero from GHND
                    lpHeader->RefCnt     = 1;
                    lpHeader->NELM       = aplNELM;
                    lpHeader->Rank       = aplRank;
#undef  lpHeader

                    // Copy the dimensions to the result
                    CopyMemory (VarArrayBaseToDim (lpMemRes),
                                VarArrayBaseToDim (lpHeader),
                                (UINT) aplRank * sizeof (APLDIM));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbArr); lpMemArr = NULL;

                    // We no longer need this storage
                    DbgGlobalFree (hGlbArr); hGlbArr = NULL;

                    // Copy the global handle
                    hGlbArr = hGlbTmp;

                    goto NORMAL_EXIT;

                defstop
                    break;
            } // End SWITCH

            // Convert the chars to blanks
            Assert (aplNELM EQ (UINT) aplNELM);
            FillMemoryW (lpMemArr, (UINT) aplNELM, L' ');

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemArr)

            // To make a prototype APA, set its
            //   offset and multiplier to zero
            lpAPA->Off =
            lpAPA->Mul = 0;

#undef  lpAPA

            break;

        case ARRAY_NESTED:
            // Empty nested arrays have one element (the prototype)
            aplNELM = max (aplNELM, 1);

            // Fall through to common HETERO/NESTED code

        case ARRAY_HETERO:
            for (u = 0; bRet && u < aplNELM; u++)
            // Split cases based upon the element's ptr type
            switch (GetPtrTypeInd (lpMemArr))
            {
                case PTRTYPE_STCONST:
                    lpSymArr = *(LPAPLHETERO) lpMemArr;

                    // It's an immediate
                    Assert (lpSymArr->stFlags.Imm);

                    // Split cases based upon the immediate type
                    switch (lpSymArr->stFlags.ImmType)
                    {
                        case IMMTYPE_BOOL:
                        case IMMTYPE_INT:
                        case IMMTYPE_FLOAT:
                            lpSymRes = GetSteZero ();

                            break;

                        case IMMTYPE_CHAR:
                            // Split cases based upon mpEnum
                            switch (mpEnum)
                            {
                                case MP_CHARS:
                                    lpSymRes = GetSteBlank ();

                                    break;

                                case MP_NUMONLY:
                                    goto DOMAIN_ERROR_EXIT;

                                    break;

                                case MP_NUMCONV:
                                    lpSymRes = GetSteZero ();

                                    break;

                                defstop
                                    break;
                            } // End SWITCH

                            break;

                        defstop
                            goto ERROR_EXIT;
                    } // End SWITCH

                    if (lpSymRes)
                        // Save back into array
                        *((LPAPLHETERO) lpMemArr)++ = lpSymRes;
                    else
                        goto SYMTAB_ERROR_EXIT;
                    break;

                case PTRTYPE_HGLOBAL:
                    Assert (aplType EQ ARRAY_NESTED);

                    // It's a valid HGLOBAL array
                    Assert (IsGlbTypeVarInd (lpMemArr));

                    hGlbTmp =
                    MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemArr),   // Proto arg handle
                                         lptkFunc,                      // Ptr to function token
                                         mpEnum);                       // Pass flag through
                    if (hGlbTmp)
                    {
                        // We no longer need this storage
                        FreeResultGlobalVar (ClrPtrTypeIndGlb (lpMemArr)); *((LPAPLNESTED) lpMemArr) = NULL;

                        *((LPAPLNESTED) lpMemArr)++ = MakeGlbTypeGlb (hGlbTmp);
                    } else
                        bRet = FALSE;
                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

DOMAIN_ERROR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);

    goto ERROR_EXIT;

SYMTAB_ERROR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME,
                               lptkFunc);
ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbArr); lpMemArr = NULL;

    if (!bRet)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbArr); hGlbArr = NULL;
    } // End IF

    DBGLEAVE;

    return hGlbArr;
} // End MakeMonPrototype_EM
#undef  APPEND_NAME


//***************************************************************************
//  MakeDydPrototype_EM
//
//  Make a prototype from a dyadic scalar function between
//    two global memory objects
//
//  Instead of returning one of the args as the result,
//    we create a result by trundling through the args
//    using scalar extension as well as checking for
//    RANK, LENGTH, and DOMAIN ERRORs between the args.
//
//  For example, adding empty vectors whose prototypes are
//    0 (0 0) and (0 0) 0 should produce an empty vector
//    whose prototype is (0 0) (0 0) by scalar extension.
//  Also, adding empty vectors whose prototypes are
//    (0 0) and (0 0 0) should produce a LENGTH ERROR.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeDydPrototype_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL MakeDydPrototype_EM
    (HGLOBAL   hGlbLft,                 // Left arg global memory handle (may be NULL if immediate)
     IMM_TYPES immTypeLft,              // Left arg storage type if immediate (see IMM_TYPES)
     LPTOKEN   lptkFunc,                // Ptr to function token
     HGLOBAL   hGlbRht,                 // Right arg global memory handle (may be NULL if immediate)
     IMM_TYPES immTypeRht,              // Right arg storage type if immediate (see IMM_TYPES)
     LPTOKEN   lptkAxis)                // Ptr to axis token (may be NULL)

{
    APLSTYPE    aplTypeLft,             // Left arg storage type
                aplTypeRht,             // Right ...
                aplTypeRes;             // Result ...
    APLNELM     aplNELMLft,             // Left arg NELM
                aplNELMRht,             // Right ...
                aplNELMRes,             // Result   ...
                aplNELMAxis;            // Axis value ...
    APLRANK     aplRankLft,             // Left arg rank
                aplRankRht,             // Right ...
                aplRankRes;             // Result ...
    HGLOBAL     hGlbRes = NULL,         // Result global memory handle
                hGlbAxis = NULL,        // Axis value ...
                hGlbWVec = NULL,        // Weighting vector ...
                hGlbOdo = NULL,         // Odometer ...
                hGlbSub;                // Subarray ...
    LPAPLNESTED lpMemLft = NULL,        // Ptr to left arg global memory
                lpMemRht = NULL,        // Ptr to right ...
                lpMemRes = NULL;        // Ptr to result   ...
    LPAPLDIM    lpMemDimRes;            // Ptr to result dimensions
    APLUINT     ByteRes,                // # bytes in the result
                uLft,                   // Loop counter
                uRht,                   // Loop counter
                uRes;                   // Loop counter
    APLINT      iDim;                   // Dimension loop counter
    BOOL        bBoolFn;                // TRUE iff the function is equal or not-equal
    LPPRIMSPEC  lpPrimSpec;             // Ptr to local PRIMSPEC
    LPAPLUINT   lpMemAxisHead = NULL,   // Ptr to axis values, fleshed out
                lpMemAxisTail = NULL,   // Ptr to grade up of AxisHead
                lpMemOdo = NULL,        // Ptr to odometer global memory
                lpMemWVec = NULL;       // Ptr to weighting vector ...

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    if (hGlbLft)        // If not immediate, ...
        AttrsOfGlb (hGlbLft, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    else
    {
        aplTypeLft = TranslateImmTypeToArrayType (immTypeLft);
        aplNELMLft = 1;
        aplRankLft = 0;
    } // End IF

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    if (hGlbRht)        // If not immediate, ...
        AttrsOfGlb (hGlbRht, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);
    else
    {
        aplTypeRht = TranslateImmTypeToArrayType (immTypeRht);
        aplNELMRht = 1;
        aplRankRht = 0;
    } // End IF

    // Handle scalar extension with empty other arg
    if (aplNELMLft EQ 0
     && hGlbRht EQ NULL)
        aplTypeRht = ARRAY_BOOL;
    if (aplNELMRht EQ 0
     && hGlbLft EQ NULL)
        aplTypeLft = ARRAY_BOOL;

    // The rank of the result is the larger of the two args
    //   unless one is a singleton.
    if (aplNELMLft EQ 1 && aplNELMRht NE 1 && aplRankLft > aplRankRht)
        aplRankRes = aplRankRht;
    else
    if (aplNELMLft NE 1 && aplNELMRht EQ 1 && aplRankLft < aplRankRht)
        aplRankRes = aplRankLft;
    else
        aplRankRes = max (aplRankLft, aplRankRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            return NULL;

        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLock (hGlbAxis);

        // Get pointer to the axis tail (where the [X] values are)
        lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRes;

    // Get the corresponding lpPrimSpec
    lpPrimSpec = PrimSpecTab[SymTrans (lptkFunc)];

    // Calculate the storage type of the result
    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (aplNELMLft,
                                               &aplTypeLft,
                                                lptkFunc,
                                                aplNELMRht,
                                               &aplTypeRht);
    if (aplTypeRes EQ ARRAY_ERROR)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    Assert (IsSimpleNum (aplTypeRes)
         || aplTypeRes EQ ARRAY_NESTED);

    // Lock the memory to get a ptr to it
    if (hGlbLft)
        lpMemLft = MyGlobalLock (hGlbLft);

    // Lock the memory to get a ptr to it
    if (hGlbRht)
        lpMemRht = MyGlobalLock (hGlbRht);

    // Check for RANK and LENGTH ERRORs
    if (!CheckRankLengthError_EM (aplRankRes,
                                  aplRankLft,
                                  aplNELMLft,
                                  lpMemLft,
                                  aplRankRht,
                                  aplNELMRht,
                                  lpMemRht,
                                  aplNELMAxis,
                                  lpMemAxisTail,
                                  lptkFunc))
        goto ERROR_EXIT;

    // The NELM of the result is the larger of the two args
    //   unless one is empty
    if (aplNELMLft EQ 0 || aplNELMRht EQ 0)
        aplNELMRes = 0;
    else
        aplNELMRes = max (aplNELMLft, aplNELMRht);

    // Handle APA result separately
    if (aplTypeRes EQ ARRAY_APA)
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

        // Allocate space for the result.
        // N.B. Conversion from APLUINT to UINT.
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbRes)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeRes;
////////lpHeader->Perm       = 0;               // Already zero from GHND
////////lpHeader->SysVar     = 0;               // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = aplRankRes;
#undef  lpHeader

        // Copy the dimensions to the result
        if (aplRankRes EQ aplRankLft)
            CopyMemory (VarArrayBaseToDim (lpMemRes),
                        VarArrayBaseToDim (lpMemLft),
                        (UINT) aplNELMRes * sizeof (APLDIM));
        else
            CopyMemory (VarArrayBaseToDim (lpMemRes),
                        VarArrayBaseToDim (lpMemRht),
                        (UINT) aplNELMRes * sizeof (APLDIM));
        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)
        // To make a prototype APA, set its
        //   offset and multiplier to zero
        lpAPA->Off =
        lpAPA->Mul = 0;
#undef  lpAPA

        goto NORMAL_EXIT;
    } // End IF

    // If the result is simple numeric,
    //   convert it to Boolean
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    // Get the function type
    bBoolFn = lptkFunc->tkData.tkChar EQ UTF16_EQUAL
           || lptkFunc->tkData.tkChar EQ UTF16_NOTEQUAL;

    // If the left arg is immediate, the result is the prototype of the other arg
    if (hGlbLft EQ NULL)
        hGlbRes = MakeMonPrototype_EM (hGlbRht,     // Proto arg handle
                                       lptkFunc,    // Ptr to function token
                                       bBoolFn ? MP_NUMCONV : MP_NUMONLY);
    else
    if (hGlbRht EQ NULL)
        hGlbRes = MakeMonPrototype_EM (hGlbLft,     // Proto arg handle
                                       lptkFunc,    // Ptr to function token
                                       bBoolFn ? MP_NUMCONV : MP_NUMONLY);
    else
    {
        // Neither arg is immediate

        // Allocate space for result
        if (!PrimScalarFnDydAllocate_EM (lptkFunc,
                                        &hGlbRes,
                                         lpMemLft,
                                         lpMemRht,
                                         aplRankLft,
                                         aplRankRht,
                                        &aplRankRes,
                                         aplTypeRes,
                                         aplNELMLft,
                                         aplNELMRht,
                                         aplNELMRes))
            goto ERROR_EXIT;

        // Take into account nested prototypes
        if (aplTypeLft EQ ARRAY_NESTED)
            aplNELMLft = max (aplNELMLft, 1);
        if (aplTypeRht EQ ARRAY_NESTED)
            aplNELMRht = max (aplNELMRht, 1);
        if (aplTypeRes EQ ARRAY_NESTED)
            aplNELMRes = max (aplNELMRes, 1);

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);      // ***FIXME***

        // Skip over the header to the dimensions
        lpMemDimRes = VarArrayBaseToDim (lpMemRes);

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
        lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

        // If either arg is not simple, loop through the result
        // Otherwise, the result is all zero (already filled in by GHND).
        if (!IsSimple (aplTypeLft)
         || !IsSimple (aplTypeRht))
        {
            // If the left arg is simple, the result is
            //   the prototype of the right arg
            if (IsSimple (aplTypeLft))
            {
                hGlbSub = MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemRht), // Proto arg handle
                                               lptkFunc,                    // Ptr to function token
                                               bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                if (!hGlbSub)
                    goto ERROR_EXIT;
                *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
            } else
            // If the right arg is simple, the result is
            //   the prototype of the left arg
            if (IsSimple (aplTypeRht))
            {
                hGlbSub = MakeMonPrototype_EM (ClrPtrTypeIndGlb (lpMemLft), // Proto arg handle
                                               lptkFunc,                    // Ptr to function token
                                               bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                if (!hGlbSub)
                    goto ERROR_EXIT;
                *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
            } else
            {
                // Both args are nested

                // Handle axis if present
                if (aplNELMAxis NE aplRankRes)
                {
                    //***************************************************************
                    // Allocate space for the weighting vector which is
                    //   {times}{backscan}1{drop}({rho}Z),1
                    // N.B.  Conversion from APLUINT to UINT.
                    //***************************************************************
                    ByteRes = aplRankRes * sizeof (APLUINT);
                    Assert (ByteRes EQ (UINT) ByteRes);
                    hGlbWVec = DbgGlobalAlloc (GHND, (UINT) ByteRes);
                    if (!hGlbWVec)
                    {
                        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Lock the memory to get a ptr to it
                    lpMemWVec = MyGlobalLock (hGlbWVec);

                    // Loop through the dimensions of the result in reverse
                    //   order {backscan} and compute the cumulative product
                    //   (weighting vector).
                    // Note we use a signed index variable because we're
                    //   walking backwards and the test against zero must be
                    //   made as a signed variable.
                    for (uRes = 1, iDim = aplRankRes - 1; iDim >= 0; iDim--)
                    {
                        lpMemWVec[iDim] = uRes;
                        uRes *= lpMemDimRes[iDim];
                    } // End FOR

                    //***************************************************************
                    // Allocate space for the odometer array, one value per dimension
                    //   in the right arg, with values initially all zero (thanks to GHND).
                    // N.B.  Conversion from APLUINT to UINT.
                    //***************************************************************
                    ByteRes = aplRankRes * sizeof (APLUINT);
                    Assert (ByteRes EQ (UINT) ByteRes);
                    hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteRes);
                    if (!hGlbOdo)
                    {
                        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                   lptkFunc);
                        goto ERROR_EXIT;
                    } // End IF

                    // Lock the memory to get a ptr to it
                    lpMemOdo = MyGlobalLock (hGlbOdo);
                } // End IF

                // Loop through the elements of the result
                for (uRes = 0; uRes < aplNELMRes; uRes++)
                {
                    // If there's an axis, ...
                    if (lptkAxis
                     && aplNELMAxis NE aplRankRes)
                    {
                        APLUINT uArg;       // Loop counter

                        // Loop through the odometer values accumulating
                        //   the weighted sum
                        for (uArg = 0, uRht = aplRankRes - aplNELMAxis; uRht < aplRankRes; uRht++)
                            uArg += lpMemOdo[lpMemAxisHead[uRht]] * lpMemWVec[uRht];

                        // Increment the odometer in lpMemOdo subject to
                        //   the values in lpMemDimRes
                        IncrOdometer (lpMemOdo, lpMemDimRes, NULL, aplRankRes);

                        // Use the just computed index for the argument
                        //   with the smaller rank
                        if (aplRankLft < aplRankRht)
                        {
                            uLft = uArg;
                            uRht = uRes;
                        } else
                        {
                            uRht = uArg;
                            uLft = uRes;
                        } // End IF/ELSE
                    } else
                    {
                        uLft = uRes % aplNELMLft;
                        uRht = uRes % aplNELMRht;
                    } // End IF/ELSE

                    // If the left arg element is an STE,
                    //   the result element is the prototype
                    //   of the right arg element
                    if (GetPtrTypeDir (lpMemLft[uLft]) EQ PTRTYPE_STCONST)
                    {
                        hGlbSub = MakeMonPrototype_EM (ClrPtrTypeDirGlb (lpMemRht[uRht]),   // Proto arg handle
                                                       lptkFunc,                            // Ptr to function token
                                                       bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                        if (!hGlbSub)
                            goto ERROR_EXIT;
                        *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
                    } else
                    // If the right arg element is an STE,
                    //   the result element is the prototype
                    //   of the left arg element
                    if (GetPtrTypeDir (lpMemRht[uRht]) EQ PTRTYPE_STCONST)
                    {
                        hGlbSub = MakeMonPrototype_EM (ClrPtrTypeDirGlb (lpMemLft[uLft]),   // Proto arg handle
                                                       lptkFunc,                            // Ptr to function token
                                                       bBoolFn ? MP_NUMCONV : MP_NUMONLY);
                        if (!hGlbSub)
                            goto ERROR_EXIT;
                        *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
                    } else
                    {
                        // Both args are nested HGLOBALs
                        hGlbSub = MakeDydPrototype_EM (ClrPtrTypeDirGlb (lpMemLft[uLft]),   // Left arg proto handle
                                                       0,                                   // Left arg immediate type (irrelevant as it's an HGLOBAL)
                                                       lptkFunc,                            // Ptr to function token
                                                       ClrPtrTypeDirGlb (lpMemRht[uRht]),   // Right arg proto handle
                                                       0,                                   // Right arg immediate type (irrelevant as it's an HGLOBAL)
                                                       NULL);
                        if (!hGlbSub)
                            goto ERROR_EXIT;
                        *lpMemRes++ = MakeGlbTypeGlb (hGlbSub);
                    } // End IF/ELSE/...
                } // End FOR
            } // End IF/ELSE/...
        } // End IF
    } // End IF/ELSE/...

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRes)
    {
        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbWVec)
    {
        if (lpMemWVec)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        if (lpMemOdo)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    if (hGlbAxis)
    {
        if (lpMemAxisHead)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxisHead = lpMemAxisTail = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
    } // End IF

    return hGlbRes;
} // End MakeDydPrototype_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IsFirstSimpleGlb
//
//  Is the first element of a global simple?
//***************************************************************************

BOOL IsFirstSimpleGlb
    (HGLOBAL *lphGlbRht,
     LPCHAR   lpaplTypeRes)

{
    LPVOID     lpMemRht;
    APLSTYPE   aplTypeRht;
    APLRANK    aplRankRht;
    BOOL       bRet = TRUE;
    LPSYMENTRY lpSym;
    HGLOBAL    hGlbFirst;

    // It's a valid HGLOBAL array
    Assert (IsGlbTypeVarDir (*lphGlbRht));

    *lphGlbRht = ClrPtrTypeDirGlb (*lphGlbRht);

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (*lphGlbRht);

#define lpHeaderRht     ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type and Rank
    aplTypeRht = lpHeaderRht->ArrType;
    aplRankRht = lpHeaderRht->Rank;
#undef  lpHeaderRht

    // It's a nested element
    Assert (aplTypeRht EQ ARRAY_NESTED);

    // Point to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Split cases based upon the element's ptr type
    switch (GetPtrTypeInd (lpMemRht))
    {
        case PTRTYPE_STCONST:       // a {is} 1 {diamond} 0{rho}a (2 3)
            lpSym = *(LPAPLHETERO) lpMemRht;

            // It's an immediate
            Assert (lpSym->stFlags.Imm);
            *lpaplTypeRes = TranslateImmTypeToArrayType (lpSym->stFlags.ImmType);

            break;

        case PTRTYPE_HGLOBAL:
            // Get the HGLOBAL
            hGlbFirst = *(LPAPLNESTED) lpMemRht;

            bRet = FALSE;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRht); lpMemRht = NULL;

    // If not simple, return the HGLOBAL of the first element
    if (!bRet)
        *lphGlbRht = hGlbFirst;

    return bRet;
} // End IsFirstSimpleGlb


//***************************************************************************
//  $CopySymGlbDir
//
//  Copy a direct LPSYMENTRY or HGLOBAL incrementing the reference count
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopySymGlbDir"
#else
#define APPEND_NAME
#endif

HGLOBAL CopySymGlbDir
    (LPVOID lpSymGlb)

{
    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (lpSymGlb))
    {
        case PTRTYPE_STCONST:
            return lpSymGlb;

        case PTRTYPE_HGLOBAL:
            // Increment the reference count in global memory
            DbgIncrRefCntDir ((HGLOBAL) lpSymGlb);

            return lpSymGlb;

        defstop
            return NULL;
    } // End SWITCH
} // End CopySymGlbDir
#undef  APPEND_NAME


//***************************************************************************
//  $CopySymGlbInd
//
//  Copy an indirect LPSYMENTRY or HGLOBAL incrementing the reference count
//***************************************************************************

HGLOBAL CopySymGlbInd
    (LPVOID lpSymGlb)

{
    return CopySymGlbDir (*(LPAPLNESTED) lpSymGlb);
} // End CopySymGlbInd


//***************************************************************************
//  $CopySymGlbDirGlb
//
//  Copy a direct HGLOBAL incrementing the reference count
//***************************************************************************

HGLOBAL CopySymGlbDirGlb
    (HGLOBAL hGlb)

{
    return CopySymGlbDir (MakeGlbTypeGlb (hGlb));
} // End CopySymGlbDirGlb


//***************************************************************************
//  $CopyArray_EM
//
//  Make a copy of a global memory ptr array
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CopyArray_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL CopyArray_EM
    (HGLOBAL hGlbSrc,       // Source handle
     LPTOKEN lptkFunc)      // Ptr to function token

{
    DWORD       dwSize;
    LPVOID      lpMemSrc, lpMemSrcBase,
                lpMemDst, lpMemDstBase;
    HGLOBAL     hGlbDst,
                hGlbTmp;
    APLSTYPE    aplType;
    APLNELM     aplNELM;
    APLRANK     aplRank;
    LPSYMENTRY  lpSymSrc,
                lpSymDst;
    APLNELM     u;
    BOOL        bRet = TRUE;

    // Get the size of the global memory object
    dwSize = MyGlobalSize (hGlbSrc);
    Assert (dwSize NE 0);

    hGlbDst = DbgGlobalAlloc (GHND, dwSize);
    if (hGlbDst)
    {
        // Lock both memory blocks
        lpMemDst = MyGlobalLock (hGlbDst);
        lpMemSrc = MyGlobalLock (hGlbSrc);

        // Copy source to destin
        CopyMemory (lpMemDst, lpMemSrc, dwSize);

        // Set the reference count in case it was a TKT_VARNAMED
#ifdef DEBUG_REFCNT
        dprintfW (L"##RefCnt=1 in " APPEND_NAME L": %08X (%S#%d)", lpMemDst, FNLN);
#endif
#define lpHeaderDst ((LPVARARRAY_HEADER) lpMemDst)
        lpHeaderDst->RefCnt = 1;

        // Recurse through the array, copying all the global ptrs
        aplType = lpHeaderDst->ArrType;
        aplNELM = lpHeaderDst->NELM;
        aplRank = lpHeaderDst->Rank;
#undef  lpHeaderDst

        lpMemDstBase = lpMemDst = VarArrayBaseToData (lpMemDst, aplRank);
        lpMemSrcBase = lpMemSrc = VarArrayBaseToData (lpMemSrc, aplRank);

        // Split cases based upon the array type
        switch (aplType)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_CHAR:
            case ARRAY_APA:
                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                // Handle the empty case
                aplNELM = max (aplNELM, 1);

                // Fill nested result with PTR_REUSED
                //   in case we fail part way through
                for (u = 0; u < aplNELM; u++)
                    *((LPAPLNESTED) lpMemDst)++ = PTR_REUSED;

                // Start the destin ptr over again
                lpMemDst = lpMemDstBase;

                // Loop through the source and destin arrays
                for (u = 0;
                     u < aplNELM;
                     u++, ((LPAPLNESTED) lpMemDst)++, ((LPAPLNESTED) lpMemSrc)++)
                // Split cases based upon the ptr type
                switch (GetPtrTypeInd (lpMemSrc))
                {
                    case PTRTYPE_STCONST:
                        lpSymSrc = *(LPAPLHETERO) lpMemSrc;

                        // It's an immediate
                        Assert (lpSymSrc->stFlags.Imm);

                        // Copy it
                        lpSymDst = CopyImmSymEntry_EM (lpSymSrc,
                                                       -1,
                                                       lptkFunc);
                        if (lpSymDst)
                            // Save into the destin
                            *((LPAPLHETERO) lpMemDst) = lpSymDst;
                        else
                            bRet = FALSE;
                        break;

                    case PTRTYPE_HGLOBAL:
                        Assert (aplType EQ ARRAY_NESTED);

                        // It's a valid HGLOBAL array
                        Assert (IsGlbTypeVarInd (lpMemSrc));

                        // Copy the array
                        hGlbTmp = CopyArray_EM (ClrPtrTypeIndGlb (lpMemSrc),
                                                lptkFunc);
                        if (hGlbTmp)
                            // Save into the destin
                            *((LPAPLNESTED) lpMemDst) = MakeGlbTypeGlb (hGlbTmp);
                        else
                            bRet = FALSE;
                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need these ptrs
        MyGlobalUnlock (hGlbDst); lpMemDst = NULL;
        MyGlobalUnlock (hGlbSrc); lpMemSrc = NULL;

        if (!bRet)
        {
            FreeResultGlobalVar (hGlbDst); hGlbDst = NULL;
        } // End IF
    } else
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
    return hGlbDst;
} // End CopyArray_EM
#undef  APPEND_NAME


#ifdef DEBUG
//***************************************************************************
//  $IsGlobalTypeArray
//
//  Confirm that an HGLOBAL is a valid variable or function array, or a
//    user-defined function/operator.
//***************************************************************************

BOOL IsGlobalTypeArray
    (HGLOBAL hGlb,
     UINT    Signature)

{
    LPVOID lpMem;
    BOOL   bRet = TRUE;

    // It's an HGLOBAL
    switch (GetPtrTypeDir (hGlb))
    {
        case PTRTYPE_STCONST:
            return FALSE;

        case PTRTYPE_HGLOBAL:
            break;

        defstop
            break;
    } // End SWITCH

    // Clear the ptr type bits
    hGlb = ClrPtrTypeDirGlb (hGlb);

    // It's a valid ptr
    bRet = bRet && IsGlbPtr (hGlb);

    if (bRet)
    {
        // Lock the memory to get a ptr to it
        lpMem = GlobalLock (hGlb); Assert (lpMem NE NULL);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
        // Ensure it has the correct signature
        bRet = (lpHeader->Sig.nature EQ Signature)
            && (lpHeader->Perm || lpHeader->RefCnt > 0);
#undef  lpHeader

        // We no longer need this ptr
        GlobalUnlock (hGlb); lpMem = NULL;
    } // End IF

    return bRet;
} // End IsGlobalTypeArray
#endif


//***************************************************************************
//  $CheckRankLengthError_EM
//
//  Check a dyadic scalar function for RANK and LENGTH ERRORs
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckRankLengthError_EM"
#else
#define APPEND_NAME
#endif

BOOL CheckRankLengthError_EM
    (APLRANK  aplRankRes,           // Result rank
     APLRANK  aplRankLft,           // Left arg ...
     APLNELM  aplNELMLft,           // ...      NELM
     LPVOID   lpMemLft,             // Ptr to left arg memory
     APLRANK  aplRankRht,           // Right arg rank
     APLNELM  aplNELMRht,           // ...       NELM
     LPVOID   lpMemRht,             // Ptr to right arg memory
     APLNELM  aplNELMAxis,          // Axis NELM
     LPAPLINT lpMemAxisTail,        // Ptr to grade up of AxisHead
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    APLINT uRes;

    //***************************************************************
    // If the ranks don't match, then it gets complicated:
    //
    //   If both arguments are singletons, the result rank
    //     (and shape) is the same rank (and shape) as the
    //     argument with the larger rank.
    //   If only one argument is a singleton, the result rank
    //     (and shape) is the same as the rank (and shape) of the
    //     non-singleton argument.
    //   If either argument is a singleton, any valid axis values
    //     are allowed.  That is, any scalar or vector X such that
    //     {and}/X{epsilon}{iota}{rho}{rho}Z.
    //   If no axis is specified, it is equivalent to the axis being
    //     {iota}{rho}{rho}Z.
    //   If neither argument is a singleton and axis is specified,
    //     then it must be true that
    //       ({rho}L)[X]{match}{rho}R
    //     assuming that L is the argument of lower rank.  If not,
    //     switch L and R in the above expression.
    //***************************************************************

    // OUTER RANK and LENGTH ERRORs are possible only if
    //   neither argument is a singleton
    if (aplNELMLft NE 1 && aplNELMRht NE 1)
    {
        // Check for OUTER RANK ERROR
        if ((aplRankLft <  aplRankRht && (aplRankLft NE aplNELMAxis))
         || (aplRankLft >  aplRankRht && (aplRankRht NE aplNELMAxis)))
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            return FALSE;
        } // End IF

        // If axis not full, ...
        if (aplNELMAxis NE aplRankRes)
        {
            if (aplRankLft < aplRankRht)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankLft; uRes++)
                if ((VarArrayBaseToDim (lpMemLft))[uRes] !=
                    (VarArrayBaseToDim (lpMemRht))[lpMemAxisTail[uRes]])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
            if (aplRankLft > aplRankRht)
            {
                // Check for OUTER LENGTH ERROR
                for (uRes = 0; uRes < (APLRANKSIGN) aplRankRht; uRes++)
                if ((VarArrayBaseToDim (lpMemLft))[lpMemAxisTail[uRes]] !=
                    (VarArrayBaseToDim (lpMemRht))[uRes])
                {
                    uRes = (APLINT) -1; // Mark as in error

                    break;
                } // End FOR/IF
            } else
                uRes = (APLINT) -1; // Mark as in error (equal ranks with axis)
        } else
        // If axis full (or no axis) and ranks the same, ...
        if (aplRankLft EQ aplRankRht)
        {
            // Check for OUTER LENGTH ERROR
            for (uRes = 0; uRes < (APLRANKSIGN) aplRankRes; uRes++)
            if ((VarArrayBaseToDim (lpMemLft))[uRes] !=
                (VarArrayBaseToDim (lpMemRht))[uRes])
            {
                uRes = (APLINT) -1; // Mark as in error

                break;
            } // End FOR/IF
        } else
            uRes = (APLINT) -1; // Mark as in error (unequal ranks w/o axis)

        // Check for error
        if (uRes EQ (APLINT) -1)
        {
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkFunc);
            return FALSE;
        } // End IF
    } // End IF

    return TRUE;
} // End CheckRankLengthError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $RoundUpBits8
//
//  Round up bits to a byte (8 bits/byte) and convert to # bytes
//***************************************************************************

APLINT RoundUpBits8
    (APLNELM aplNELM)           // NELM to convert

{
    return ((aplNELM + (NBIB - 1)) & ~MASKLOG2NBIB) >> LOG2NBIB;
} // End RoundUpBits8


//***************************************************************************
//  $RoundUpBitsInArray
//
//  Round up bits to a dword (32 bits/dword) and convert to # bytes
//***************************************************************************

APLINT RoundUpBitsInArray
    (APLNELM aplNELM)           // NELM to convert

{
    return ((aplNELM + (NBID - 1)) & ~MASKLOG2NBID) >> LOG2NBIB;
} // End RoundUpBitsInArray


//***************************************************************************
//  $abs64
//
//  Return the absolute value of a 64-bit integer
//
//  Originally, this function was a macro as in
//
//    #define abs64(a)    (((a)>0)?a:-(a))
//
//  however, the first time I tried
//
//    abs64 (*((LPAPLINT) lpMemLft)++)
//
//  I decided it was better off as a function.
//***************************************************************************

APLINT abs64
    (APLINT aplInt)

{
    return (aplInt > 0) ? aplInt : -aplInt;
} // End abs64


//***************************************************************************
//  $imul64
//
//  Multiply two 64-bit integers retaining maximum precision
//***************************************************************************

APLINT imul64
    (APLINT aplLft,
     APLINT aplRht,
     LPBOOL lpbRet)     // Is the result valid?? (may be NULL)

{
    APLINT aplRes;

    _clear87 ();

    _asm
    {
        fild    aplLft;
        fild    aplRht;
        fmulp   st(1),st(0);
        fistp   aplRes;
    }

    // Check for overflow
    if (_SW_INVALID & _status87 ())
    {
        if (lpbRet)
            *lpbRet = FALSE;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return aplRes;
} // End imul64


//***************************************************************************
//  $CalcArraySize
//
//  Calculate the size in bytes of an array
//***************************************************************************

APLUINT CalcArraySize
    (ARRAY_TYPES aplType,       // Result storage type
     APLNELM     aplNELM,       // ...    NELM
     APLRANK     aplRank)       // ...    rank

{
    APLUINT ByteRes;            // # bytes in the result

    Assert (0 <= aplType
         &&      aplType < ARRAY_LENGTH);

    // Split cases based upon the result storage type
    switch (aplType)
    {
        case ARRAY_BOOL:
            ByteRes = sizeof (APLBOOL)   * RoundUpBitsInArray (aplNELM);

            break;

        case ARRAY_INT:
            ByteRes = sizeof (APLINT)    * aplNELM;

            break;

        case ARRAY_FLOAT:
            ByteRes = sizeof (APLFLOAT)  * aplNELM;

            break;

        case ARRAY_CHAR:
            ByteRes = sizeof (APLCHAR)   * aplNELM;

            break;

        case ARRAY_APA:
            ByteRes = sizeof (APLAPA);

            break;

        case ARRAY_HETERO:
            ByteRes = sizeof (APLHETERO) * aplNELM;

            break;

        case ARRAY_LIST:
            ByteRes = sizeof (APLNESTED) * aplNELM;

            break;

        case ARRAY_NESTED:
            // Make room for the prototype
            ByteRes = sizeof (APLNESTED) * max (aplNELM, 1);

            break;

        defstop
            break;
    } // End SWITCH

    // Add in the size of the header and dimension
    ByteRes += CalcHeaderSize (aplRank);

    return ByteRes;
} // End CalcArraySize


//***************************************************************************
//  $CalcHeaderSize
//
//  Calculate the size in bytes of the header of an array
//***************************************************************************

APLUINT CalcHeaderSize
    (APLRANK aplRank)

{
    return sizeof (VARARRAY_HEADER)
         + sizeof (APLDIM) * aplRank;
} // End CalcHeaderSize


//***************************************************************************
//  $MakeNoValue_YY
//
//  Return a NoValue PL_YYSTYPE entry
//***************************************************************************

LPPL_YYSTYPE MakeNoValue_YY
    (LPTOKEN lptkFunc)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARNAMED;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;             // Already zero from YYAlloc
    lpYYRes->tkToken.tkFlags.NoDisplay = 1;
    lpYYRes->tkToken.tkData.tkSym      = lpMemPTD->steNoValue;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpYYRes;
} // End MakeNoValue_YY


//***************************************************************************
//  $GetSISLayer
//
//  Peel back to the first non-Imm/Exec layer
//***************************************************************************

LPSIS_HEADER GetSISLayer
    (LPSIS_HEADER lpSISCur)

{
    while (lpSISCur
        && (lpSISCur->DfnType EQ DFNTYPE_IMM
         || lpSISCur->DfnType EQ DFNTYPE_EXEC))
        lpSISCur = lpSISCur->lpSISPrv;

    return lpSISCur;
} // End GetSISLayer


//***************************************************************************
//  $FillSISNxt
//
//  Fill in the SIS Header for lpSISNxt
//***************************************************************************

void FillSISNxt
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     HANDLE       hSemaphore,           // Semaphore handle
     DFN_TYPES    DfnType,              // DFNTYPE_xxx
     FCN_VALENCES FcnValence,           // FCNVALENCE_xxx
     BOOL         Suspended,            // TRUE iff starts Suspended
     BOOL         LinkIntoChain)        // TRUE iff we should link this entry into the SIS chain

{
    lpMemPTD->SILevel++;

RESTART_EXCEPTION_FILLSISNXT:
    __try
    {
        // Create another level on the SI stack
        lpMemPTD->lpSISNxt->Sig.nature    = SIS_HEADER_SIGNATURE;
        lpMemPTD->lpSISNxt->hSemaphore    = hSemaphore;
        lpMemPTD->lpSISNxt->hSigaphore    = NULL;
        lpMemPTD->lpSISNxt->hGlbDfnHdr    = NULL;
        lpMemPTD->lpSISNxt->hGlbFcnName   = NULL;
        lpMemPTD->lpSISNxt->hGlbQuadEM    = hGlbM3x0Char;
        lpMemPTD->lpSISNxt->DfnType       = DfnType;
        lpMemPTD->lpSISNxt->FcnValence    = FcnValence;
        lpMemPTD->lpSISNxt->DfnAxis       = FALSE;
        lpMemPTD->lpSISNxt->Suspended     = Suspended;
        lpMemPTD->lpSISNxt->Perm          = FALSE;
        lpMemPTD->lpSISNxt->Avail         = 0;
        lpMemPTD->lpSISNxt->EventType     = 0;
        lpMemPTD->lpSISNxt->CurLineNum    = 0;
        lpMemPTD->lpSISNxt->NxtLineNum    = 0;
        lpMemPTD->lpSISNxt->numLabels     = 0;
////    lpMemPTD->lpSISNxt->numResultSTE  = 0;
////    lpMemPTD->lpSISNxt->numLftArgSTE  = 0;
////    lpMemPTD->lpSISNxt->numRhtArgSTE  = 0;
////    lpMemPTD->lpSISNxt->numLocalsSTE  = 0;
        lpMemPTD->lpSISNxt->numFcnLines   = 0;
        lpMemPTD->lpSISNxt->QQPromptLen   = 0;
        lpMemPTD->lpSISNxt->ErrorCode     = ERRORCODE_NONE;
        lpMemPTD->lpSISNxt->lpSISPrv      = lpMemPTD->lpSISCur;
        lpMemPTD->lpSISNxt->lpSISNxt      = (LPSIS_HEADER) ByteAddr (lpMemPTD->lpSISNxt, sizeof (SIS_HEADER));
#ifdef DEBUG
        dprintfW (L"~~Localize:    %08X (%s)", lpMemPTD->lpSISNxt, L"FillSISNxt");
#endif
        // Link this SIS into the chain, if requested
        if (LinkIntoChain)
        {
            if (lpMemPTD->lpSISCur)
                lpMemPTD->lpSISCur->lpSISNxt = lpMemPTD->lpSISNxt;
            lpMemPTD->lpSISCur = lpMemPTD->lpSISNxt;
            lpMemPTD->lpSISNxt = lpMemPTD->lpSISNxt->lpSISNxt;
        } // End IF
    } __except (CheckException (GetExceptionInformation (), "FillSISNxt"))
    {
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_ACCESS_VIOLATION:
            {
                MEMORY_BASIC_INFORMATION mbi;

                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                // See how many pages are already allocated
                VirtualQuery (lpMemPTD->lpSISNxt,
                             &mbi,
                              sizeof (mbi));

                // Check for no allocation as yet
                if (mbi.State EQ MEM_RESERVE)
                    mbi.RegionSize = 0;

                // Allocate more memory to the YYRes buffer
                if (VirtualAlloc (lpMemPTD->lpSISNxt,
                                  mbi.RegionSize + DEF_SIS_INCRSIZE * sizeof (SYMENTRY),
                                  MEM_COMMIT,
                                  PAGE_READWRITE) NE NULL)
                    goto RESTART_EXCEPTION_FILLSISNXT;

                // Fall through to never-never-land

            } // End EXCEPTION_ACCESS_VIOLATION

            default:
                // Display message for unhandled exception
                DisplayException ();
#ifdef DEBUG
                DbgStop ();         // We should never get here
#endif
                break;
        } // End SWITCH
    } // End __try/__except
} // End FillSISNxt


//***************************************************************************
//  $PassSigaphore
//
// If this hSigaphore is not for this level, pass it on up the line
//***************************************************************************

LPSIS_HEADER PassSigaphore
    (LPPERTABDATA lpMemPTD,
     HANDLE       hSigaphore)
{
    if (hSigaphore
     && lpMemPTD->lpSISCur
     && hSigaphore NE lpMemPTD->lpSISCur->hSemaphore)
    {
        Assert (lpMemPTD->lpSISCur->hSigaphore EQ NULL);

        // Pass it on up the line
        lpMemPTD->lpSISCur->hSigaphore = hSigaphore;
        hSigaphore = NULL;
    } // End IF

    return hSigaphore;
} // End PassSigaphore


//***************************************************************************
//  $GetImmTypeFcn
//
//  Get the IMMTYPE_PRIMFCN value corresponding to a primitive function
//***************************************************************************

UINT GetImmTypeFcn
    (WCHAR wch)                 // Immediate function symbol (UTF16_xxx)

{
    return IMMTYPE_PRIMFCN;

////// Split cases based upon the immediate function symbol
////switch (wch)
////{
////    case UTF16_BAR:
////    case UTF16_CIRCLE:
////    case UTF16_CIRCLESTAR:
////    case UTF16_COLONBAR:
////    case UTF16_DOWNSTILE:
////    case UTF16_PLUS:
////    case UTF16_QUOTEDOT:
////    case UTF16_STAR:
////    case UTF16_STILE:
////    case UTF16_TIMES:
////    case UTF16_UPSTILE:
////        return IMMTYPE_PRIMFCN_SS;      // Monadic scalar     , dyadic scalar
////
////    case UTF16_QUERY:
////    case UTF16_TILDE:
////        return IMMTYPE_PRIMFCN_SM;      // Monadic scalar     , dyadic mixed/error
////
////    case UTF16_DOWNCARET:
////    case UTF16_DOWNCARETTILDE:
////    case UTF16_EQUAL:
////    case UTF16_LEFTCARET:
////    case UTF16_LEFTCARETUNDERBAR:
////    case UTF16_NOTEQUAL:
////    case UTF16_RIGHTCARET:
////    case UTF16_RIGHTCARETUNDERBAR:
////    case UTF16_UPCARET:
////    case UTF16_UPCARETTILDE:
////        return IMMTYPE_PRIMFCN_MS;      // Monadic mixed/error, dyadic scalar
////
////    case UTF16_ALPHA:
////    case UTF16_CIRCLEBAR:
////    case UTF16_CIRCLESLOPE:
////    case UTF16_CIRCLESTILE:
////    case UTF16_COMMA:
////    case UTF16_COMMABAR:
////    case UTF16_DELSTILE:
////    case UTF16_DELTASTILE:
////    case UTF16_DOMINO:
////    case UTF16_DOWNARROW:
////    case UTF16_DOWNTACK:
////    case UTF16_DOWNTACKJOT:
////    case UTF16_EPSILON:
////    case UTF16_EPSILONUNDERBAR:
////    case UTF16_EQUALUNDERBAR:
////    case UTF16_IOTA:
////    case UTF16_IOTAUNDERBAR:
////    case UTF16_LEFTSHOE:
////    case UTF16_LEFTTACK:
////    case UTF16_OMEGA:
////    case UTF16_RIGHTSHOE:
////    case UTF16_RIGHTTACK:
////    case UTF16_RHO:
////    case UTF16_SLASH:
////    case UTF16_SLASHBAR:
////    case UTF16_SLOPE:
////    case UTF16_SLOPEBAR:
////    case UTF16_SQUAD:
////    case UTF16_UPARROW:
////    case UTF16_UPSHOE:
////    case UTF16_UPTACK:
////    case UTF16_UPTACKJOT:
////        return IMMTYPE_PRIMFCN_MM;      // Monadic mixed/error, dyadic mixed/error
////
////    defstop
////        return 0;
////} // End SWITCH
} // End GetImmTypeFcn


//***************************************************************************
//  End of File: primfns.c
//***************************************************************************
