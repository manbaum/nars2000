//***************************************************************************
//  NARS2000 -- Translate Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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
#include "qf_ec.h"


//***************************************************************************
//  $TranslateSOTypeToNameType
//
//  Translate a SynObj type (see SO_ENUM)
//    to a function type (see NAME_TYPES).
//***************************************************************************

NAME_TYPES TranslateSOTypeToNameType
    (SO_ENUM soType)            // SynObj type

{
    // Split cases based upon the Syntax Object type
    switch (soType)
    {
        // Flavors of vars
        case soA:
        case soSA:
        case soSPA:
        case soIDX:
            return NAMETYPE_VAR;

        // Flavors of monadic/dyadic functions
        case soF:
        case soFR:
        case soFFR:
        case soSPF:
        case soSPFR:
        case soMF:
            return NAMETYPE_FN12;

        // Flavors of niladic functions
        case soNF:
        case soSPNF:
            return NAMETYPE_FN0;

        // Flavors of monadic operators
        case soMOP:
        case soMOPN:
        case soJD:
        case soSPM:
        case soSPMN:
        case soSPMR:
            return NAMETYPE_OP1;

        // Flavors of dyadic operators
        case soDOP:
        case soDOPN:
        case soSPD:
        case soSPDN:
        case soSPDR:
            return NAMETYPE_OP2;

        // Flavors of hybrids
        case soHY:
        case soSPHY:
        case soSPHR:
            return NAMETYPE_OP3;

        defstop
            return NAMETYPE_UNK;
    } // End SWITCH
} // End TranslateSOTypeToNameType


//***************************************************************************
//  $TranslateNameTypeToSOType
//
//  Translate a function type (see NAME_TYPES).
//    to a SynObj type (see SO_ENUM)
//***************************************************************************

SO_ENUM TranslateNameTypeToSOType
    (NAME_TYPES nameType)

{
    // Split cases based upon the name type
    switch (nameType)
    {
        case NAMETYPE_VAR:
            return soA;

        case NAMETYPE_FN0:
            return soNF;

        case NAMETYPE_FN12:
        case NAMETYPE_TRN:
            return soF;

        case NAMETYPE_OP1:
            return soMOP;

        case NAMETYPE_OP2:
            return soDOP;

        case NAMETYPE_OP3:
            return soHY;

        case NAMETYPE_UNK:
        case NAMETYPE_LST:

        defstop
            return soUNK;
    } // End SWITCH
} // End TranslateNameTypeToSOType


//***************************************************************************
//  $TranslateDfnTypeToSOType
//
//  Translate a UDFO type (see FH_PARSE.H)
//    to a SynObj type (see SO_ENUM)
//***************************************************************************

SO_ENUM TranslateDfnTypeToSOType
    (LPDFN_HEADER lpMemDfnHdr)

{
    Assert (GetSignatureMem (lpMemDfnHdr) EQ DFN_HEADER_SIGNATURE);

    // Split cases based upon the DfnType
    switch (lpMemDfnHdr->DfnType)
    {
        case DFNTYPE_OP1:
            return (lpMemDfnHdr->FcnValence EQ FCNVALENCE_NIL) ? soMOPN : soMOP;

        case DFNTYPE_OP2:
            return (lpMemDfnHdr->FcnValence EQ FCNVALENCE_NIL) ? soDOPN : soDOP;

        case DFNTYPE_FCN:
            return (lpMemDfnHdr->FcnValence EQ FCNVALENCE_NIL) ? soNF   : soF  ;

        case DFNTYPE_UNK:
        defstop
            return soUNK;
    } // End SWITCH
} // End TranslateDfnTypeToSOType


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
     FCN_VALENCES fcnValence)   // Function valence (see FCN_VALENCES)

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

        case IMMTYPE_RAT:
        case IMMTYPE_VFP:
            return TKT_VARARRAY;

        defstop
            return IMMTYPE_ERROR;   // To keep the compiler happy
    } // End SWITCH
} // End TranslateImmTypeToTknType


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

        case IMMTYPE_RAT:
            return STRAND_RAT;

        case IMMTYPE_VFP:
            return STRAND_VFP;

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

        case IMMTYPE_RAT:
            return ARRAY_RAT;

        case IMMTYPE_VFP:
            return ARRAY_VFP;

        case IMMTYPE_HC2I:
            return ARRAY_HC2I;

        case IMMTYPE_HC2F:
            return ARRAY_HC2F;

        case IMMTYPE_HC2R:
            return ARRAY_HC2R;

        case IMMTYPE_HC2V:
            return ARRAY_HC2V;

        case IMMTYPE_HC4I:
            return ARRAY_HC4I;

        case IMMTYPE_HC4F:
            return ARRAY_HC4F;

        case IMMTYPE_HC4R:
            return ARRAY_HC4R;

        case IMMTYPE_HC4V:
            return ARRAY_HC4V;

        case IMMTYPE_HC8I:
            return ARRAY_HC8I;

        case IMMTYPE_HC8F:
            return ARRAY_HC8F;

        case IMMTYPE_HC8R:
            return ARRAY_HC8R;

        case IMMTYPE_HC8V:
            return ARRAY_HC8V;

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

        case ARRAY_RAT:
            return IMMTYPE_RAT;

        case ARRAY_VFP:
            return IMMTYPE_VFP;

        case ARRAY_HC2I:
            return IMMTYPE_HC2I;

        case ARRAY_HC2F:
            return IMMTYPE_HC2F;

        case ARRAY_HC2R:
            return IMMTYPE_HC2R;

        case ARRAY_HC2V:
            return IMMTYPE_HC2V;

        case ARRAY_HC4I:
            return IMMTYPE_HC4I;

        case ARRAY_HC4F:
            return IMMTYPE_HC4F;

        case ARRAY_HC4R:
            return IMMTYPE_HC4R;

        case ARRAY_HC4V:
            return IMMTYPE_HC4V;

        case ARRAY_HC8I:
            return IMMTYPE_HC8I;

        case ARRAY_HC8F:
            return IMMTYPE_HC8F;

        case ARRAY_HC8R:
            return IMMTYPE_HC8R;

        case ARRAY_HC8V:
            return IMMTYPE_HC8V;

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

        case ARRAY_RAT:
            return STRAND_RAT;

        case ARRAY_VFP:
            return STRAND_VFP;

        case ARRAY_HC2I:
            return STRAND_HC2I;

        case ARRAY_HC2F:
            return STRAND_HC2F;

        case ARRAY_HC2R:
            return STRAND_HC2R;

        case ARRAY_HC2V:
            return STRAND_HC2V;

        case ARRAY_HC4I:
            return STRAND_HC4I;

        case ARRAY_HC4F:
            return STRAND_HC4F;

        case ARRAY_HC4R:
            return STRAND_HC4R;

        case ARRAY_HC4V:
            return STRAND_HC4V;

        case ARRAY_HC8I:
            return STRAND_HC8I;

        case ARRAY_HC8F:
            return STRAND_HC8F;

        case ARRAY_HC8R:
            return STRAND_HC8R;

        case ARRAY_HC8V:
            return STRAND_HC8V;

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

        case STRAND_RAT:
            return ARRAY_RAT;

        case STRAND_VFP:
            return ARRAY_VFP;

        case STRAND_HC2I:
            return ARRAY_HC2I;

        case STRAND_HC2F:
            return ARRAY_HC2F;

        case STRAND_HC2R:
            return ARRAY_HC2R;

        case STRAND_HC2V:
            return ARRAY_HC2V;

        case STRAND_HC4I:
            return ARRAY_HC4I;

        case STRAND_HC4F:
            return ARRAY_HC4F;

        case STRAND_HC4R:
            return ARRAY_HC4R;

        case STRAND_HC4V:
            return ARRAY_HC4V;

        case STRAND_HC8I:
            return ARRAY_HC8I;

        case STRAND_HC8F:
            return ARRAY_HC8F;

        case STRAND_HC8R:
            return ARRAY_HC8R;

        case STRAND_HC8V:
            return ARRAY_HC8V;

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

        case 'R':
            return ARRAY_RAT;

        case 'V':
            return ARRAY_VFP;

        case 'i':
            return ARRAY_HC2I;

        case 'f':
            return ARRAY_HC2F;

        case 'r':
            return ARRAY_HC2R;

        case 'v':
            return ARRAY_HC2V;

        case 'j':
            return ARRAY_HC4I;

        case 'g':
            return ARRAY_HC4F;

        case 's':
            return ARRAY_HC4R;

        case 'w':
            return ARRAY_HC4V;

        case 'k':
            return ARRAY_HC8I;

        case 'h':
            return ARRAY_HC8F;

        case 't':
            return ARRAY_HC8R;

        case 'x':
            return ARRAY_HC8V;

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

        case INDEX_OPSLASH:                     // 0x02           // ...       {slash} as an operator
            return UTF16_SLASH;

        case INDEX_OPSLASHBAR:                  // 0x03           // ...       {slashbar} ...
            return UTF16_SLASHBAR;

        case INDEX_OPSLOPE:                     // 0x04           // ...       {slope}    ...
            return UTF16_SLOPE;

        case INDEX_OPSLOPEBAR:                  // 0x05           // ...       {slopebar} ...
            return UTF16_SLOPEBAR;

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
        // If the char is UpCaret, ...
        if (*lpMemRht EQ UTF16_UPCARET)
            *lpMemRht++ = L'^';
        else
        // If the char is Tilde, ...
        if (*lpMemRht EQ UTF16_TILDE)
            *lpMemRht++ = L'~';
        else
        // If the char is Stile, ...
        if (*lpMemRht EQ UTF16_STILE)
            *lpMemRht++ = L'|';
        else
        // If the char is Bar2, ...
        if (*lpMemRht EQ UTF16_BAR2)
            *lpMemRht++ = L'-';
        else
        // If the char is Quad2, ...
        if (*lpMemRht EQ UTF16_QUAD2)
            *lpMemRht++ = UTF16_QUAD;
        else
            *lpMemRht++ = L'\xFF';
    } // End WHILE
} // End TranslateNARSToAPL2


//***************************************************************************
//  $TranslateExitTypeToReturnCode
//
//  Translate EXITTYPE_xxx to []EC return code
//***************************************************************************

UINT TranslateExitTypeToReturnCode
    (EXIT_TYPES exitType)

{
    // Split cases based upon the EXITTYPE_xxx
    switch (exitType)
    {
        case EXITTYPE_GOTO_ZILDE:
        case EXITTYPE_GOTO_LINE:
            return EC_RETCODE_GOTO_LINE;

        case EXITTYPE_ERROR:
        case EXITTYPE_STACK_FULL:
        case EXITTYPE_QUADERROR_INIT:
            return EC_RETCODE_ERROR;

        case EXITTYPE_DISPLAY:
            return EC_RETCODE_DISPLAY;

        case EXITTYPE_NODISPLAY:
            return EC_RETCODE_NODISPLAY;

        case EXITTYPE_NOVALUE:
            return EC_RETCODE_NOVALUE;

        case EXITTYPE_RESET_ONE_INIT:
        case EXITTYPE_RESET_ONE:
            return EC_RETCODE_RESET_ONE;

        case EXITTYPE_NONE:
        case EXITTYPE_RESET_ALL:
        case EXITTYPE_QUADERROR_EXEC:
        case EXITTYPE_STOP:
        defstop
            return EC_RETCODE_UNK;
    } // End SWITCH
} // End TranslateExitTypeToReturnCode


//***************************************************************************
//  $TranslateQuadICIndex
//
//  Translate a given []IC index into an APLFLOAT return value
//***************************************************************************

APLFLOAT TranslateQuadICIndex
    (APLFLOAT   aplFloatLft,        // Left arg
     IC_INDICES icIndex,            // []IC index
     APLFLOAT   aplFloatRht,        // Right arg
     UBOOL      bNegate)            // TRUE iff we should negate result

{
    // Split cases based upon the []IC index value
    switch (GetQuadICValue (icIndex))
    {
        case ICVAL_NEG1:
            return bNegate ? 1 : -1;

        case ICVAL_ZERO:
            return (gAllowNeg0 && bNegate) ? -0.0 : 0.0;

        case ICVAL_ONE:
            return bNegate ? -1 : 1;

        case ICVAL_DOMAIN_ERROR:
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case ICVAL_POS_INFINITY:
            return bNegate ? fltNegInfinity : fltPosInfinity;

        case ICVAL_NEG_INFINITY:
            return bNegate ? fltPosInfinity : fltNegInfinity;

        case ICVAL_LEFT:
            return bNegate ? -aplFloatLft : aplFloatLft;

        case ICVAL_RIGHT:
            return bNegate ? -aplFloatRht : aplFloatRht;

        defstop
            return (gAllowNeg0 && bNegate) ? -0.0 : 0.0;
    } // End SWITCH
} // TranslateQuadICIndex


//***************************************************************************
//  $TranslateCharToType
//
//  Translate am ASCIIZ APLCHAR vector to a storage type
//***************************************************************************

APLSTYPE TranslateCharToType
    (LPAPLCHAR lpMemChar)

{
    if (lstrcmpiW (lpMemChar, L"INT" ) EQ 0)
        return ARRAY_INT;
    else
    if (lstrcmpiW (lpMemChar, L"FLT" ) EQ 0)
        return ARRAY_FLOAT;
    else
    if (lstrcmpiW (lpMemChar, L"RAT" ) EQ 0)
        return ARRAY_RAT;
    else
    if (lstrcmpiW (lpMemChar, L"VFP" ) EQ 0)
        return ARRAY_VFP;
    else
    if (lstrcmpiW (lpMemChar, L"HC2I") EQ 0)
        return ARRAY_HC2I;
    else
    if (lstrcmpiW (lpMemChar, L"HC2F") EQ 0)
        return ARRAY_HC2F;
    else
    if (lstrcmpiW (lpMemChar, L"HC2R") EQ 0)
        return ARRAY_HC2R;
    else
    if (lstrcmpiW (lpMemChar, L"HC2V") EQ 0)
        return ARRAY_HC2V;
    else
    if (lstrcmpiW (lpMemChar, L"HC4I") EQ 0)
        return ARRAY_HC4I;
    else
    if (lstrcmpiW (lpMemChar, L"HC4F") EQ 0)
        return ARRAY_HC4F;
    else
    if (lstrcmpiW (lpMemChar, L"HC4R") EQ 0)
        return ARRAY_HC4R;
    else
    if (lstrcmpiW (lpMemChar, L"HC4V") EQ 0)
        return ARRAY_HC4V;
    else
    if (lstrcmpiW (lpMemChar, L"HC8I") EQ 0)
        return ARRAY_HC8I;
    else
    if (lstrcmpiW (lpMemChar, L"HC8F") EQ 0)
        return ARRAY_HC8F;
    else
    if (lstrcmpiW (lpMemChar, L"HC8R") EQ 0)
        return ARRAY_HC8R;
    else
    if (lstrcmpiW (lpMemChar, L"HC8V") EQ 0)
        return ARRAY_HC8V;
    else
        return ARRAY_ERROR;
} // End TranslateCharToType


//***************************************************************************
//  $TranslateExceptionCodeToArrayType
//
//  Translate an ExceptionCode (see EXCEPTION_CODES) to
//    an array type (see ARRAY_TYPES)
//***************************************************************************

APLSTYPE TranslateExceptionCodeToArrayType
    (EXCEPTION_CODES exCode)

{
    // Split cases based upon the ExceptionCode
    switch (exCode)
    {
        case EXCEPTION_RESULT_FLOAT:
            return ARRAY_FLOAT;

        case EXCEPTION_RESULT_VFP:
            return ARRAY_VFP;

        case EXCEPTION_RESULT_HC2F:
            return ARRAY_HC2F;

        case EXCEPTION_RESULT_HC4F:
            return ARRAY_HC4F;

        case EXCEPTION_RESULT_HC8F:
            return ARRAY_HC8F;

        case EXCEPTION_RESULT_HC2V:
            return ARRAY_HC2V;

        case EXCEPTION_RESULT_HC4V:
            return ARRAY_HC4V;

        case EXCEPTION_RESULT_HC8V:
            return ARRAY_HC8V;

        defstop
            return ARRAY_ERROR;
    } // End SWITCH
} // End TranslateExceptionCodeToArrayType


//***************************************************************************
//  End of File: translate.c
//***************************************************************************
