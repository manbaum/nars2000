//***************************************************************************
//  NARS2000 -- Symbol Translation Routine & Tables
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $FcnTrans
//
//  Return the index corresponding to the UTF16_xxx function
//***************************************************************************

UINT FcnTrans
    (WCHAR wchFcn)          // Function symbol

{
    // Split cases based upon the function symbol
    switch (wchFcn)
    {
////////case UTF16_DOWNSTILE:               // 0x230A
////////    return (UCHAR) wchFcn;          // Use the low-order byte

        case UTF16_EPSILON:                 // 0x220A
            return INDEX_EPSILON;           // Use special index

////////case UTF16_STAR:                    // 0x002A
////////    return (UCHAR) wchFcn;          // Use the low-order byte

        case UTF16_DOWNSHOE:                // 0x222A
            return INDEX_DOWNSHOE;          // Use special index

////////case UTF16_EQUAL:                   // 0x003D
////////    return (UCHAR) wchFcn;          // Use the low-order byte

        case UTF16_CIRCLESTILE:             // 0x233D
            return INDEX_CIRCLESTILE;       // Use special index

////////case UTF16_QUERY:                   // 0x003F
////////    return (UCHAR) wchFcn;          // Use the low-order byte

        case UTF16_SLASHBAR:                // 0x233F
            return INDEX_SLASHBAR;          // Use special index

////////case UTF16_EQUALUNDERBAR:           // 0x2261
////////    return (UCHAR) wchFcn;          // Use the low-order byte

////////case UTF16_DIERESISDOWNTACK:        // 0x2361
////////    return INDEX_DIERESISDOWNTACK;  // Use special index

////////case UTF16_LEFTCARETUNDERBAR:       // 0x2264
////////    return (UCHAR) wchFcn;          // Use the low-order byte

////////case UTF16_DIERESISJOT:             // 0x2364
////////    return INDEX_DIERESISJOT;       // Use special index

////////case UTF16_RIGHTCARETUNDERBAR:      // 0x2265
////////    return (UCHAR) wchFcn;          // Use the low-order byte

////////case UTF16_DIERESISCIRCLE:          // 0x2365
////////    return INDEX_DIERESISCIRCLE;    // Use special index

        default:                            // All others
            return (UCHAR) wchFcn;          // Use the low-order byte
    } // End SWITCH
} // End FcnTrans


//***************************************************************************
//  $OprTrans
//
//  Return the index corresponding to the UTF16_xxx operator
//***************************************************************************

UINT OprTrans
    (WCHAR wchOpr)          // Operator symbol

{
    // Split cases based upon the operator symbol
    switch (wchOpr)
    {
        case UTF16_SLASH:                   // 0x002F
            return INDEX_OPSLASH;           // Use special index

        case UTF16_SLASHBAR:                // 0x233F
            return INDEX_OPSLASHBAR;        // Use special index

        case UTF16_SLOPE:                   // 0x005C
            return INDEX_OPSLOPE;           // Use special index

        case UTF16_SLOPEBAR:                // 0x2340
            return INDEX_OPSLOPEBAR;        // Use special index

        case UTF16_DIERESISDOWNTACK:        // 0x2361
            return INDEX_DIERESISDOWNTACK;  // Use special index

        case UTF16_DIERESISJOT:             // 0x2364
            return INDEX_DIERESISJOT;       // Use special index

        case UTF16_DIERESISCIRCLE:          // 0x2365
            return INDEX_DIERESISCIRCLE;    // Use special index

        case UTF16_JOTDOT:                  // 0x0001
            return INDEX_JOTDOT;            // Use special index

        default:                            // All others
            return (UCHAR) wchOpr;          // Use the low-order byte
    } // End SWITCH
} // End OprTrans


//***************************************************************************
//  $SymTrans
//
//  Return the index corresponding to the UTF16_xxx symbol
//***************************************************************************

UINT SymTrans
    (LPTOKEN lptkFunc)      // Ptr to function token

{
    WCHAR wch;              // The symbol

    Assert (lptkFunc->tkFlags.TknType EQ TKT_FCNIMMED
         || lptkFunc->tkFlags.TknType EQ TKT_OP1IMMED
         || lptkFunc->tkFlags.TknType EQ TKT_OP2IMMED
         || lptkFunc->tkFlags.TknType EQ TKT_OP3IMMED
         || lptkFunc->tkFlags.TknType EQ TKT_OPJOTDOT);

    // Get the symbol
    wch = lptkFunc->tkData.tkChar;

    // Split cases based upon the token type
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            return FcnTrans (wch);

        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
        case TKT_OPJOTDOT:
            return OprTrans (wch);

        defstop
            return 0;
    } // End SWITCH
} // End SymTrans


//***************************************************************************
//  End of File: symtrans.c
//***************************************************************************
