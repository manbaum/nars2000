//***************************************************************************
//  NARS2000 -- Translate Routines
//***************************************************************************

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "externs.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $TranslateTknTypeToTknTypeNamed
//
//  Translate a token type (see TOKENTYPES enum) to
//    a named token type (see TOKENTYPES enum).
//***************************************************************************

TOKENTYPES TranslateTknTypeToTknTypeNamed
    (TOKENTYPES tknType)

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

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateTknTypeToTknTypeNamed


//***************************************************************************
//  TranslateDfnToNameType
//
//  Translate a user-defined function/operator type (see enum DFN_TYPES and enum FCN_VALENCES)
//    to a function type (see enum NAME_TYPES).
//***************************************************************************

NAME_TYPES TranslateDfnToNameType
    (DFN_TYPES    dfnType,      // User-defined function/operator type (see enum DFN_TYPES)
     FCN_VALENCES fcnValence)   // Function valance (see enum FCN_VALENCES)

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
//  $TranslateImmTypeToTknType
//
//  Translate an immediate type (see IMMTYPES enum) to
//    a token type (see TOKENTYPES enum).
//***************************************************************************

TOKENTYPES TranslateImmTypeToTknType
    (IMMTYPES immType)

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

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateImmTypeToTknType


//// //***************************************************************************
//// //  $TranslateImmTypeToTknTypeNamed
//// //
//// //  Translate an immediate type (see IMMTYPES enum) to
//// //    a named token type (see TOKENTYPES enum).
//// //***************************************************************************
////
//// TOKENTYPES TranslateImmTypeToTknTypeNamed
////     (IMMTYPES immType)
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
////         defstop
////             return -1;              // To keep the compiler happy
////     } // End SWITCH
//// } // End TranslateImmTypeToTknTypeNamed


//***************************************************************************
//  $TranslateImmTypeToStrandType
//
//  Translate an immediate type (see IMMTYPES enum) to
//    a strand type (see STRAND_TYPES enum).
//***************************************************************************

STRAND_TYPES TranslateImmTypeToStrandType
    (IMMTYPES immType)

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
//  Translate an immediate type (see IMMTYPES enum) to
//    an array type (see ARRAY_TYPES enum).
//***************************************************************************

ARRAY_TYPES TranslateImmTypeToArrayType
    (IMMTYPES immType)

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
//  Translate an array type (see ARRAY_TYPES enum) to
//    an immediate type (see IMMTYPES enum).
//***************************************************************************

IMMTYPES TranslateArrayTypeToImmType
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
//  Translate an array type (see ARRAY_TYPES enum) to
//    a strand type (see STRAND_TYPES enum).
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
//  Translate a strand type (see STRAND_TYPES enum) to
//    an array type (see ARRAY_TYPES enum).
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
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateStrandTypeToArrayType


//***************************************************************************
//  End of File: translate.c
//***************************************************************************
