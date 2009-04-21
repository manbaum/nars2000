//***************************************************************************
//  NARS2000 -- Translate Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
#include <stdio.h>
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $TranslateTknTypeToTknTypeNamed
//
//  Translate a token type (see TOKEN_TYPES) to
//    a named token type (see TOKEN_TYPES).
//***************************************************************************

TOKEN_TYPES TranslateTknTypeToTknTypeNamed
    (TOKEN_TYPES tknType)

{
    switch (tknType)
    {
        case TKT_VARNAMED:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            return TKT_VARNAMED;

        case TKT_FCNNAMED:
        case TKT_FCNIMMED:
        case TKT_FCNARRAY:
            return TKT_FCNNAMED;

        case TKT_OP1NAMED:
        case TKT_OP1IMMED:
            return TKT_OP1NAMED;

        case TKT_OP2NAMED:
        case TKT_OP2IMMED:
            return TKT_OP2NAMED;

        case TKT_OP3NAMED:
        case TKT_OP3IMMED:
            return TKT_OP3NAMED;

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateTknTypeToTknTypeNamed


//***************************************************************************
//  TranslateDfnToNameType
//
//  Translate a user-defined function/operator type (see DFN_TYPES and FCN_VALENCES)
//    to a function type (see NAME_TYPES).
//***************************************************************************

NAME_TYPES TranslateDfnToNameType
    (DFN_TYPES    dfnType,      // User-defined function/operator type (see DFN_TYPES)
     FCN_VALENCES fcnValence)   // Function valance (see FCN_VALENCES)

{
    // Split cases based upon the user-defined function/operator type
    switch (dfnType)
    {
        case DFNTYPE_OP1:
            return NAMETYPE_OP1;

        case DFNTYPE_OP2:
            return NAMETYPE_OP2;

        case DFNTYPE_FCN:
            // Split cases based upon the function valence
            switch (fcnValence)
            {
                case FCNVALENCE_NIL:
                    return NAMETYPE_FN0;

                case FCNVALENCE_MON:
                case FCNVALENCE_DYD:
                case FCNVALENCE_AMB:
                    return NAMETYPE_FN12;

                defstop
                    return NAMETYPE_UNK;
            } // End SWITCH

        case DFNTYPE_UNK:
        defstop
            return NAMETYPE_UNK;
    } // End SWITCH
} // End TranslateDfnToNameType


//***************************************************************************
//  $TranslateNameTypeToImmType
//
//  Translate a name type (see NAME_TYPES) to
//    an immediate type (see IMM_TYPES).
//***************************************************************************

IMM_TYPES TranslateNameTypeToImmType
    (NAME_TYPES nameType)

{
    switch (nameType)
    {
        case NAMETYPE_FN12:
            return IMMTYPE_PRIMFCN;

        case NAMETYPE_OP1:
            return IMMTYPE_PRIMOP1;

        case NAMETYPE_OP2:
            return IMMTYPE_PRIMOP2;

        case NAMETYPE_OP3:
            return IMMTYPE_PRIMOP3;

        case NAMETYPE_FN0:
        case NAMETYPE_UNK:
        case NAMETYPE_VAR:
        case NAMETYPE_FILL1:
        case NAMETYPE_LST:
        case NAMETYPE_TRN:
        defstop
            return IMMTYPE_ERROR;
    } // End SWITCH
} // End TranslateNameTypeToImmType


//***************************************************************************
//  $TranslateImmTypeToTknType
//
//  Translate an immediate type (see IMM_TYPES) to
//    a token type (see TOKEN_TYPES).
//***************************************************************************

TOKEN_TYPES TranslateImmTypeToTknType
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

        case IMMTYPE_PRIMOP3:
            return TKT_OP3IMMED;

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateImmTypeToTknType


//// //***************************************************************************
//// //  $TranslateImmTypeToTknTypeNamed
//// //
//// //  Translate an immediate type (see IMM_TYPES) to
//// //    a named token type (see TOKEN_TYPES).
//// //***************************************************************************
////
//// TOKEN_TYPES TranslateImmTypeToTknTypeNamed
////     (IMM_TYPES immType)
////
//// {
////     switch (immType)
////     {
////         case IMMTYPE_BOOL:
////         case IMMTYPE_INT:
////         case IMMTYPE_CHAR:
////         case IMMTYPE_FLOAT:
////             return TKT_VARNAMED;
////
////         case IMMTYPE_PRIMFCN:
////             return TKT_FCNNAMED;
////
////         case IMMTYPE_PRIMOP1:
////             return TKT_OP1NAMED;
////
////         case IMMTYPE_PRIMOP2:
////             return TKT_OP2NAMED;
////
////         case IMMTYPE_PRIMOP3:
////             return TKT_OP3NAMED;
////
////         defstop
////             return -1;              // To keep the compiler happy
////     } // End SWITCH
//// } // End TranslateImmTypeToTknTypeNamed


//***************************************************************************
//  $TranslateImmTypeToStrandType
//
//  Translate an immediate type (see IMM_TYPES) to
//    a strand type (see STRAND_TYPES).
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
//  $TranslateImmTypeToArrayType
//
//  Translate an immediate type (see IMM_TYPES) to
//    an array type (see ARRAY_TYPES).
//***************************************************************************

ARRAY_TYPES TranslateImmTypeToArrayType
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
//  $TranslateArrayTypeToImmType
//
//  Translate an array type (see ARRAY_TYPES) to
//    an immediate type (see IMM_TYPES).
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
            return IMMTYPE_ERROR;

        defstop
            return IMMTYPE_ERROR;       // To keep the compiler happy
    } // End SWITCH
} // End TranslateArrayTypeToImmType


//***************************************************************************
//  $TranslateArrayTypeToStrandType
//
//  Translate an array type (see ARRAY_TYPES) to
//    a strand type (see STRAND_TYPES).
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
//  $TranslateStrandTypeToArrayType
//
//  Translate a strand type (see STRAND_TYPES) to
//    an array type (see ARRAY_TYPES).
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
            return ARRAY_ERROR;     // To keep the compiler happy
    } // End SWITCH
} // End TranslateStrandTypeToArrayType


//***************************************************************************
//  $TranslateResetFlagToExitType
//
//  Translate a ResetFlag (see RESET_FLAGS) to
//    an ExitType (see EXIT_TYPES).
//***************************************************************************

EXIT_TYPES TranslateResetFlagToExitType
    (RESET_FLAGS resetFlag)

{
    // Split cases based upon the reset flag
    switch (resetFlag)
    {
        case RESETFLAG_NONE:
            return EXITTYPE_NONE;

        case RESETFLAG_ONE:
            return EXITTYPE_RESET_ONE;

        case RESETFLAG_ONE_INIT:
            return EXITTYPE_RESET_ONE_INIT;

        case RESETFLAG_QUADERROR_INIT:
            return EXITTYPE_QUADERROR_INIT;

        case RESETFLAG_QUADERROR_EXEC:
            return EXITTYPE_QUADERROR_EXEC;

        case RESETFLAG_ALL:
            return EXITTYPE_RESET_ALL;

        case RESETFLAG_STOP:
            return EXITTYPE_STOP;

        defstop
            return EXITTYPE_NONE;
    } // End SWITCH
} // End TranslateResetFlagToExitType


//***************************************************************************
//  $TranslateImmTypeToChar
//
//  Convert an immediate type to a char
//***************************************************************************

APLCHAR TranslateImmTypeToChar
    (IMM_TYPES immType)

{
#ifdef DEBUG
    APLCHAR aplChar;

    aplChar = ImmTypeAsChar[immType];

    Assert (aplChar NE L'!' && aplChar NE L'?');

    return aplChar;
#else
    return ImmTypeAsChar[immType];
#endif
} // End TranslateImmTypeToChar


//***************************************************************************
//  $TranslateArrayTypeToChar
//
//  Convert an array type to a char
//***************************************************************************

APLCHAR TranslateArrayTypeToChar
    (ARRAY_TYPES arrType)

{
    return ArrayTypeAsChar[arrType];
} // End TranslateArrayTypeToChar


//***************************************************************************
//  $TranslateCharToArrayType
//
//  Convert a char to an array type
//***************************************************************************

ARRAY_TYPES TranslateCharToArrayType
    (WCHAR wChar)

{
    // Split cases based upon the incoming char
    switch (wChar)
    {
        case 'A':
            return ARRAY_APA;

        case 'B':
            return ARRAY_BOOL;

        case 'C':
            return ARRAY_CHAR;

        case 'F':
            return ARRAY_FLOAT;

        case 'H':
            return ARRAY_HETERO;

        case 'I':
            return ARRAY_INT;

        case 'N':
            return ARRAY_NESTED;

        defstop
            return '?';
    } // End SWITCH
} // End TranslateCharToArrayType


//***************************************************************************
//  $TranslateFcnOprToChar
//
//  Translate an immediate function/operator to an UTF16_xxx char
//***************************************************************************

APLCHAR TranslateFcnOprToChar
    (APLCHAR fcnOpr)            // The operator/function to translate

{
    // Split cases based upon the symbol
    switch (fcnOpr)
    {
        case INDEX_JOTDOT:                      // 0x01           // Index for {jot}{dot}
            // We should never get here
            DbgStop ();

            return UTF16_JOT;

        case INDEX_EPSILON:                     // 0x02           // ...       {epsilon}
            return UTF16_EPSILON;

        case INDEX_DOWNSHOE:                    // 0x03           // ...       {downshoe}
            return UTF16_DOWNSHOE;

        case INDEX_CIRCLESTILE:                 // 0x04           // ...       {circlestile}
            return UTF16_CIRCLESTILE;

        case INDEX_SLASHBAR:                    // 0x05           // ...       {slashbar}
            return UTF16_SLASHBAR;

////////case                                    // 0x06           // ...       {delta}
////////case                                    // 0x07           // ...       {del}
////////case                                    // 0x08           // ...       {upstile}
        case INDEX_DIERESISJOT:                 // 0x09           // ...       {dieresisjot}
            return UTF16_DIERESISJOT;

////////case                                    // 0x0A           // ...       {epsilon}/{downstile}
        case INDEX_DIERESISDOWNTACK:            // 0x0B           // ...       {dieresisdowntack}
            return UTF16_DIERESISDOWNTACK;

        case INDEX_OPSLASH:                     // 0x0C           // ...       {slash} as an operator
            return UTF16_SLASH;

        case INDEX_OPSLASHBAR:                  // 0x0D           // ...       {slashbar} ...
            return UTF16_SLASHBAR;

        case INDEX_OPSLOPE:                     // 0x0E           // ...       {slope}    ...
            return UTF16_SLOPE;

        case INDEX_OPSLOPEBAR:                  // 0x0F           // ...       {slopebar} ...
            return UTF16_SLOPEBAR;

////////case                                    // 0x10-0x17      // (available)
////////case                                    // 0x18           // ...       {jot}

        default:
            return fcnOpr;
    } // End SWITCH
} // End TranslateFcnOprToChar


//***************************************************************************
//  $TranslateTabIDToIndex
//
//  Translate a tab ID (unique global number) to
//            a tab index (a value the Tab Control understands)
//***************************************************************************

int TranslateTabIDToIndex
    (int iTabID)

{
    int iTabCount;                  // The # tabs

    // Get the # tabs
    iTabCount = TabCtrl_GetItemCount (hWndTC);

    // Loop through each tab looking for a match between the lpMemPTD
    while (--iTabCount >= 0)
    if (iTabID EQ TranslateTabIndexToID (iTabCount))
        return iTabCount;

    // Mark as failed
    return -1;
} // End TranslateTabIDToIndex


//***************************************************************************
//  $TranslateTabIndexToID
//
//  Translate a tab index (a value the Tab Control understands) to
//            a tab ID (unique global number).
//***************************************************************************

int TranslateTabIndexToID
    (int iTabIndex)

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetPerTabPtr (iTabIndex); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // If the incoming index is invalid or there's no lpMemPTD, ...
    if (iTabIndex EQ -1 || !IsValidPtr (lpMemPTD, sizeof (lpMemPTD)))
        return -1;

    // Return the matching tab ID
    return lpMemPTD->CurTabID;
} // End TranslateTabIndexToID


//***************************************************************************
//  $TranslateAPL2ToNARS
//
//  Translate from the APL2 charset to NARS
//***************************************************************************

void TranslateAPL2ToNARS
    (LPWCHAR lpMemRht,                  // Ptr to data to translate
     APLNELM aplNELMRht,                // Length of ...
     UBOOL   bIsEBCDIC)                 // TRUE iff the input is in EBCDIC format

{
    LPWCHAR lpwTranslate;               // Ptr to either APL2_ASCIItoNARS or APL2_EBCDICtoNARS

    // Set the translate table
    lpwTranslate = (bIsEBCDIC ? APL2_EBCDICtoNARS : APL2_ASCIItoNARS);

    // Loop through the elements
    while (aplNELMRht--)
        *lpMemRht++ = lpwTranslate[*lpMemRht];
} // End TranslateAPL2ToNARS


//***************************************************************************
//  $TranslateNARSToAPL2
//
//  Translate from the NARS charset to APL2
//***************************************************************************

void TranslateNARSToAPL2
    (LPWCHAR lpMemRht,                  // Ptr to data to translate
     APLNELM aplNELMRht,                // Length of ...
     UBOOL   bIsEBCDIC)                 // TRUE iff the input is in EBCDIC format

{
    LPWCHAR lpwTranslate,               // Ptr to either APL2_ASCIItoNARS or APL2_EBCDICtoNARS
            lpwTemp;

    // Set the translate table
    lpwTranslate = (bIsEBCDIC ? APL2_EBCDICtoNARS : APL2_ASCIItoNARS);

    // Skip over the first char as it's always zero
    lpwTranslate++;

    // Loop through the elements
    while (aplNELMRht--)
    {
        lpwTemp = strchrW (lpwTranslate, *lpMemRht);
        if (lpwTemp)
            *lpMemRht++ = (WCHAR) (lpwTemp - &lpwTranslate[-1]);
        else
            *lpMemRht++ = L'\xFF';
    } // End WHILE
} // End TranslateNARSToAPL2


//***************************************************************************
//  End of File: translate.c
//***************************************************************************
