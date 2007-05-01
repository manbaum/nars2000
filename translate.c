//***************************************************************************
//  NARS2000 -- Translate Routines
//***************************************************************************

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  TranslateFcnTypeToTknType
//
//  Translate a function type (see FCN_TYPES enum) to
//    a named token type (see TOKEN_TYPES enum).
//***************************************************************************

STRAND_TYPES TranslateFcnTypeToTknTypeNamed
    (FCN_TYPES fcnType)

{
    switch (fcnType)
    {
        case FCNTYPE_FCN:
        case FCNTYPE_AXISFCN:
            return TKT_FCNNAMED;

        case FCNTYPE_OP1:
            return TKT_OP1NAMED;

        case FCNTYPE_OP2:
            return TKT_OP2NAMED;

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateFcnTypeToTknTypeNamed


//***************************************************************************
//  TranslateImmTypeToTknType
//
//  Translate an immediate type (see IMM_TYPES enum) to
//    a token type (see TOKEN_TYPES enum).
//***************************************************************************

STRAND_TYPES TranslateImmTypeToTknType
    (IMM_TYPES immType)

{
    switch (immType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
        case IMMTYPE_CHAR:
        case IMMTYPE_FLOAT:
            return TKT_VARIMMED;

        case IMMTYPE_PRIMFCN_MM:
        case IMMTYPE_PRIMFCN_MS:
        case IMMTYPE_PRIMFCN_SM:
        case IMMTYPE_PRIMFCN_SS:
            return TKT_FCNIMMED;

        case IMMTYPE_PRIMOP1:
            return TKT_OP1IMMED;

        case IMMTYPE_PRIMOP2:
            return TKT_OP2IMMED;

        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateImmTypeToTknType


//***************************************************************************
//  TranslateImmTypeToStrandType
//
//  Translate an immediate type (see IMM_TYPES enum) to
//    a strand type (see STRAND_TYPES enum).
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
//  TranslateImmTypeToArrayType
//
//  Translate an immediate type (see IMM_TYPES enum) to
//    an array type (see ARRAY_TYPES enum).
//***************************************************************************

APLSTYPE TranslateImmTypeToArrayType
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
//  TranslateArrayTypeToImmType
//
//  Translate an array type (see ARRAY_TYPES enum) to
//    an immediate type (see IMM_TYPES enum).
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
        defstop
            return -1;              // To keep the compiler happy
    } // End SWITCH
} // End TranslateArrayTypeToImmType


//***************************************************************************
//  TranslateArrayTypeToStrandType
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
//  TranslateStrandTypeToArrayType
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
