//***************************************************************************
//  NARS2000 -- System Function -- Quad AV
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
//  $SysFnAV_EM
//
//  System function:  []AV -- Atomic Vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnAV_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnAV_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbAV);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnAV_EM
#undef  APPEND_NAME


//***************************************************************************
//  $MakeQuadAV
//
//  Create the global memory value for []AV
//***************************************************************************

void MakeQuadAV
    (void)

{
    LPVARARRAY_HEADER lpHeader;     // Ptr to array header
    LPAPLCHAR         lpMemRes;     // Ptr to result global memory
    UINT              uRes;         // Loop counter

    // Create []AV
    hGlbAV = MyGlobalAlloc (GHND, (UINT) CalcArraySize (ARRAY_CHAR, 256, 1));
    if (!hGlbAV)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbAV);

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
    lpHeader->Perm       = 1;       // So we don't free it
////lpHeader->SysVar     = 0;       // Already zero from GHND
////lpHeader->RefCnt     = 0;       // Ignore as this is perm
    lpHeader->NELM       = 256;
    lpHeader->Rank       = 1;

    // Fill in the dimension
    *VarArrayBaseToDim (lpHeader) = 256;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpHeader, 1);

    // Fill in the result:  0-0x7E
    for (uRes = 0; uRes < 0x7F;  uRes++)
        lpMemRes[uRes] = uRes;

    // Fill in the result:  0x7F-0xFF
    for (        ; uRes < 0x100; uRes++)
        lpMemRes[uRes] = L' ';

    // Fill in the result:  miscellaneous 0x80-0xFF
    lpMemRes[0x7F] = UTF16_ZILDE;
    lpMemRes[0x80] = 0x00C7;            // C cedilla
    lpMemRes[0x81] = 0x00FC;            // u umlaut
    lpMemRes[0x82] = 0x00E9;            // e right
    lpMemRes[0x83] = 0x00E2;            // a circumflex
    lpMemRes[0x84] = 0x00E4;            // a umlaut
    lpMemRes[0x85] = 0x00E0;            // a left
    lpMemRes[0x86] = 0x00E5;            // a circle
    lpMemRes[0x87] = 0x00E7;            // c cedilla
    lpMemRes[0x88] = 0x00EA;            // e circumflex
    lpMemRes[0x89] = 0x00EB;            // e dieresis
    lpMemRes[0x8A] = 0x00E8;            // e left
    lpMemRes[0x8B] = 0x00EF;            // i dieresis
    lpMemRes[0x8C] = 0x00EE;            // i circumflex
    lpMemRes[0x8D] = 0x00EC;            // i left
    lpMemRes[0x8E] = 0x00C4;            // A dieresis
    lpMemRes[0x8F] = 0x00C2;            // A circumflex

    lpMemRes[0x90] = UTF16_QUAD;
    lpMemRes[0x91] = UTF16_QUOTEQUAD;
    lpMemRes[0x92] = UTF16_DOMINO;
    lpMemRes[0x93] = 0x00F4;            // o circumflex
    lpMemRes[0x94] = 0x00F6;            // o dieresis
    lpMemRes[0x95] = 0x00F2;            // o left
    lpMemRes[0x96] = 0x00FC;            // u dieresis
    lpMemRes[0x97] = 0x00F9;            // u left
    lpMemRes[0x98] = UTF16_DOWNTACK;
    lpMemRes[0x99] = 0x00D6;            // O dieresis
    lpMemRes[0x9A] = 0x00DC;            // U dieresis
    lpMemRes[0x9B] = 0x00F8;            // 0 slash
    lpMemRes[0x9C] = 0x00A3;            // Pound sign
    lpMemRes[0x9D] = UTF16_UPTACK;
////lpMemRes[0x9E] = 0x20A7;            // Pts
    lpMemRes[0x9E] = 0x20AC;            // Euro
    lpMemRes[0x9F] = UTF16_IBEAM;

    lpMemRes[0xA0] = 0x00E1;            // a right
    lpMemRes[0xA1] = 0x00ED;            // i right
    lpMemRes[0xA2] = 0x00F3;            // o right
    lpMemRes[0xA3] = 0x00FA;            // u right
    lpMemRes[0xA4] = 0x00F1;            // n tilde
    lpMemRes[0xA5] = 0x00D1;            // N tilde
////lpMemRes[0xA6] = 0x00AA;            // a underbar
////lpMemRes[0xA7] = 0x00BA;            // o underbar
    lpMemRes[0xA6] = UTF16_DIERESISDEL;
    lpMemRes[0xA7] = UTF16_DIERESISSTAR;
    lpMemRes[0xA8] = 0x00BF;            // Inverted query
    lpMemRes[0xA9] = UTF16_UPSTILE;
    lpMemRes[0xAA] = 0x00AC;            // PL1 not          // Available???
    lpMemRes[0xAB] = UTF16_DIERESISTILDE;
    lpMemRes[0xAC] = UTF16_DOWNSHOE;
    lpMemRes[0xAD] = 0x00A1;            // Inverted shreik
    lpMemRes[0xAE] = UTF16_DOWNTACKJOT;
    lpMemRes[0xAF] = UTF16_UPTACKJOT;

    lpMemRes[0xB0] = 0x00  ;            // Filler           // Available???
    lpMemRes[0xB1] = 0x00  ;            // Filler           // Available???
    lpMemRes[0xB2] = 0x00  ;            // Filler           // Available???
    lpMemRes[0xB3] = UTF16_STILE;
    lpMemRes[0xB4] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xB5] = UTF16_CIRCLESTAR;
    lpMemRes[0xB6] = UTF16_DELTA;
    lpMemRes[0xB7] = UTF16_DEL;
    lpMemRes[0xB8] = UTF16_RIGHTARROW;
    lpMemRes[0xB9] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xBA] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xBB] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xBC] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xBD] = UTF16_LEFTARROW;
    lpMemRes[0xBE] = UTF16_DOWNSTILE;
    lpMemRes[0xBF] = 0x00  ;            // Line draw        // Available???

    lpMemRes[0xC0] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xC1] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xC2] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xC3] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xC4] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xC5] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xC6] = UTF16_UPARROW;
    lpMemRes[0xC7] = UTF16_DOWNARROW;
    lpMemRes[0xC8] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xC9] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xCA] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xCB] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xCC] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xCD] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xCE] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xCF] = UTF16_EQUALUNDERBAR;

    lpMemRes[0xD0] = UTF16_IOTAUNDERBAR;
    lpMemRes[0xD1] = UTF16_EPSILONUNDERBAR;
    lpMemRes[0xD2] = UTF16_DIERESISDOT;
    lpMemRes[0xD3] = UTF16_SQUAD;
    lpMemRes[0xD4] = UTF16_QUADSLOPE;
    lpMemRes[0xD5] = UTF16_QUADJOT;
    lpMemRes[0xD6] = UTF16_RIGHTTACK;
    lpMemRes[0xD7] = UTF16_LEFTTACK;
    lpMemRes[0xD8] = UTF16_DIAMOND;
    lpMemRes[0xD9] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xDA] = 0x00  ;            // Line draw        // Available???
    lpMemRes[0xDB] = 0x00  ;            // Filler           // Available???
    lpMemRes[0xDC] = 0x00  ;            // Filler           // Available???
    lpMemRes[0xDD] = 0x00  ;            // Filler           // Available???
    lpMemRes[0xDE] = 0x00  ;            // Filler           // Available???
    lpMemRes[0xDF] = 0x00  ;            // Filler           // Available???

    lpMemRes[0xE0] = UTF16_ALPHA;
    lpMemRes[0xE1] = 0x00DF;            // Beta             // Available???
    lpMemRes[0xE2] = UTF16_LEFTSHOE;
    lpMemRes[0xE3] = UTF16_RIGHTSHOE;
    lpMemRes[0xE4] = UTF16_LAMP;
    lpMemRes[0xE5] = UTF16_UPCARETTILDE;
    lpMemRes[0xE6] = UTF16_RHO;
    lpMemRes[0xE7] = UTF16_DOWNCARETTILDE;
    lpMemRes[0xE8] = UTF16_CIRCLESTILE;
    lpMemRes[0xE9] = UTF16_CIRCLEBAR;
    lpMemRes[0xEA] = UTF16_CIRCLE;
    lpMemRes[0xEB] = UTF16_DOWNCARET;
    lpMemRes[0xEC] = UTF16_IOTA;
    lpMemRes[0xED] = UTF16_CIRCLESLOPE;
    lpMemRes[0xEE] = UTF16_EPSILON;
    lpMemRes[0xEF] = UTF16_UPSHOE;

    lpMemRes[0xF0] = UTF16_SLASHBAR;
    lpMemRes[0xF1] = UTF16_SLOPEBAR;
    lpMemRes[0xF2] = UTF16_RIGHTCARETUNDERBAR;
    lpMemRes[0xF3] = UTF16_LEFTCARETUNDERBAR;
    lpMemRes[0xF4] = UTF16_NOTEQUAL;
    lpMemRes[0xF5] = UTF16_TIMES;
    lpMemRes[0xF6] = UTF16_COLONBAR;
    lpMemRes[0xF7] = UTF16_DELTAUNDERBAR;
    lpMemRes[0xF8] = UTF16_JOT;
    lpMemRes[0xF9] = UTF16_OMEGA;
    lpMemRes[0xFA] = UTF16_DELTILDE;
    lpMemRes[0xFB] = UTF16_DELTASTILE;
    lpMemRes[0xFC] = UTF16_DELSTILE;
    lpMemRes[0xFD] = UTF16_OVERBAR;
    lpMemRes[0xFE] = UTF16_DIERESIS;
    lpMemRes[0xFF] = 0x00  ;            // Blank            // Available???

////lpMemRes[0x??] = UTF16_DIERESISDOWNTACK;
////lpMemRes[0x??] = UTF16_DIERESISCIRCLE;
////lpMemRes[0x??] = UTF16_STILETILDE;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbAV); lpMemRes = NULL;
} // End MakeQuadAV


//***************************************************************************
//  End of File: qf_av.c
//***************************************************************************
