//***************************************************************************
//  NARS2000 -- System Function -- Quad AV
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
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnAV_EM_YY
//
//  System function:  []AV -- Atomic Vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnAV_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnAV_EM_YY
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
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbAV);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnAV_EM_YY
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
    APLUINT           ByteRes;      // # bytes in the result
    UINT              uRes;         // Loop counter

////#define QUADAV_LEN      (256 + 10)
#define QUADAV_LEN      APLCHAR_SIZE

    // Calculate # bytes in the result
    ByteRes = CalcArraySize (ARRAY_CHAR, QUADAV_LEN, 1);

    // Create []AV
    hGlbAV = MyGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbAV)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbAV);

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
    lpHeader->PermNdx    = PERMNDX_QUADAV;  // So we don't free it
////lpHeader->SysVar     = 0;               // Already zero from GHND
////lpHeader->RefCnt     = 0;               // Ignore as this is perm
    lpHeader->NELM       = QUADAV_LEN;
    lpHeader->Rank       = 1;

    // Fill in the dimension
    *VarArrayBaseToDim (lpHeader) = QUADAV_LEN;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpHeader, 1);

    // Fill in the result with 0-0xFFFF
    for (uRes = 0; uRes < QUADAV_LEN;  uRes++)
        lpMemRes[uRes] = uRes;
////     // Fill in the result:  0-0x7F
////     for (uRes = 0; uRes < 0x80;  uRes++)
////         lpMemRes[uRes] = uRes;
////
////     // Fill in the result:  miscellaneous 0x80-0xFF
////     // This order was takne mostly from the one used
////     //   by APL2 in their 1994 manual.
////     lpMemRes[0x080] = 0x00C7;            // C cedilla
////     lpMemRes[0x081] = 0x00FC;            // u umlaut
////     lpMemRes[0x082] = 0x00E9;            // e right
////     lpMemRes[0x083] = 0x00E2;            // a circumflex
////     lpMemRes[0x084] = 0x00E4;            // a umlaut
////     lpMemRes[0x085] = 0x00E0;            // a left
////     lpMemRes[0x086] = 0x00E5;            // a circle
////     lpMemRes[0x087] = 0x00E7;            // c cedilla
////     lpMemRes[0x088] = 0x00EA;            // e circumflex
////     lpMemRes[0x089] = 0x00EB;            // e dieresis
////     lpMemRes[0x08A] = 0x00E8;            // e left
////     lpMemRes[0x08B] = 0x00EF;            // i dieresis
////     lpMemRes[0x08C] = 0x00EE;            // i circumflex
////     lpMemRes[0x08D] = 0x00EC;            // i left
////     lpMemRes[0x08E] = 0x00C4;            // A dieresis
////     lpMemRes[0x08F] = 0x00C5;            // A circle
////
////     lpMemRes[0x090] = UTF16_QUAD;
////     lpMemRes[0x091] = UTF16_QUOTEQUAD;
////     lpMemRes[0x092] = UTF16_DOMINO;
////     lpMemRes[0x093] = 0x00F4;            // o circumflex
////     lpMemRes[0x094] = 0x00F6;            // o dieresis
////     lpMemRes[0x095] = 0x00F2;            // o left
////     lpMemRes[0x096] = 0x00FB;            // u circumflex
////     lpMemRes[0x097] = 0x00F9;            // u left
////     lpMemRes[0x098] = UTF16_DOWNTACK;
////     lpMemRes[0x099] = 0x00D6;            // O dieresis
////     lpMemRes[0x09A] = 0x00DC;            // U dieresis
////     lpMemRes[0x09B] = 0x00F8;            // 0 slash
////     lpMemRes[0x09C] = 0x00A3;            // Pound sign
////     lpMemRes[0x09D] = UTF16_UPTACK;
////     lpMemRes[0x09E] = 0x20A7;            // Pts
////     lpMemRes[0x09F] = UTF16_IBEAM;
////
////     lpMemRes[0x0A0] = 0x00E1;            // a right
////     lpMemRes[0x0A1] = 0x00ED;            // i right
////     lpMemRes[0x0A2] = 0x00F3;            // o right
////     lpMemRes[0x0A3] = 0x00FA;            // u right
////     lpMemRes[0x0A4] = 0x00F1;            // n tilde
////     lpMemRes[0x0A5] = 0x00D1;            // N tilde
////     lpMemRes[0x0A6] = 0x00AA;            // a superscript underbar
////     lpMemRes[0x0A7] = 0x00BA;            // o superscript underbar
////     lpMemRes[0x0A8] = 0x00BF;            // Inverted query
////     lpMemRes[0x0A9] = UTF16_UPSTILE;
////     lpMemRes[0x0AA] = 0x00AC;            // PL1 not
////     lpMemRes[0x0AB] = 0x0000;            // ***Available***
////     lpMemRes[0x0AC] = UTF16_DOWNSHOE;
////     lpMemRes[0x0AD] = 0x00A1;            // Inverted shreik
////     lpMemRes[0x0AE] = UTF16_DOWNTACKJOT;
////     lpMemRes[0x0AF] = UTF16_UPTACKJOT;
////
////     lpMemRes[0x0B0] = 0x2591;            // Light shade
////     lpMemRes[0x0B1] = 0x2592;            // Medium shade
////     lpMemRes[0x0B2] = 0x2593;            // Dark shade
////     lpMemRes[0x0B3] = 0x2502;            // Box drawing:  Light vertical
////     lpMemRes[0x0B4] = 0x2524;            // Box drawing:  Light vertical left
////     lpMemRes[0x0B5] = UTF16_CIRCLESTAR;
////     lpMemRes[0x0B6] = UTF16_DELTA;
////     lpMemRes[0x0B7] = UTF16_DEL;
////     lpMemRes[0x0B8] = UTF16_RIGHTARROW;
////     lpMemRes[0x0B9] = 0x2563;            // Box drawing:  Double vertical left
////     lpMemRes[0x0BA] = 0x2551;            // Box drawing:  Double vertical
////     lpMemRes[0x0BB] = 0x2557;            // Box drawing:  Double down left
////     lpMemRes[0x0BC] = 0x255D;            // Box drawing:  Double up left
////     lpMemRes[0x0BD] = UTF16_LEFTARROW;
////     lpMemRes[0x0BE] = UTF16_DOWNSTILE;
////     lpMemRes[0x0BF] = 0x2510;            // Box drawing:  Light down left
////
////     lpMemRes[0x0C0] = 0x2514;            // Box drawing:  Light up right
////     lpMemRes[0x0C1] = 0x2534;            // Box drawing:  Light up horizontal
////     lpMemRes[0x0C2] = 0x252C;            // Box drawing:  Light down horizontal
////     lpMemRes[0x0C3] = 0x251C;            // Box drawing:  Light vertical right
////     lpMemRes[0x0C4] = 0x2500;            // Box drawing:  Light horizontal
////     lpMemRes[0x0C5] = 0x253C;            // Box drawing:  Light vertical horizontal
////     lpMemRes[0x0C6] = UTF16_UPARROW;
////     lpMemRes[0x0C7] = UTF16_DOWNARROW;
////     lpMemRes[0x0C8] = 0x255A;            // Box drawing:  Double up right
////     lpMemRes[0x0C9] = 0x2554;            // Box drawing:  Double down right
////     lpMemRes[0x0CA] = 0x2569;            // Box drawing:  Double up horizontal
////     lpMemRes[0x0CB] = 0x2566;            // Box drawing:  Double down horizontal
////     lpMemRes[0x0CC] = 0x2560;            // Box drawing:  Double vertical right
////     lpMemRes[0x0CD] = 0x2550;            // Box drawing:  Double horizontal
////     lpMemRes[0x0CE] = 0x256C;            // Box drawing:  Double vertical horizontal
////     lpMemRes[0x0CF] = UTF16_EQUALUNDERBAR;
////
////     lpMemRes[0x0D0] = UTF16_IOTAUNDERBAR;
////     lpMemRes[0x0D1] = UTF16_EPSILONUNDERBAR;
////     lpMemRes[0x0D2] = UTF16_DIERESISDOT;
////     lpMemRes[0x0D3] = UTF16_SQUAD;
////     lpMemRes[0x0D4] = UTF16_QUADSLOPE;
////     lpMemRes[0x0D5] = UTF16_QUADJOT;
////     lpMemRes[0x0D6] = UTF16_RIGHTTACK;
////     lpMemRes[0x0D7] = UTF16_LEFTTACK;
////     lpMemRes[0x0D8] = UTF16_DIAMOND;
////     lpMemRes[0x0D9] = 0x2518;            // Box drawing:  Light up left
////     lpMemRes[0x0DA] = 0x250C;            // Box drawing:  Light down right
////     lpMemRes[0x0DB] = 0x2588;            // Full block
////     lpMemRes[0x0DC] = 0x2584;            // Lower half block
////     lpMemRes[0x0DD] = 0x258C;            // Left half block
////     lpMemRes[0x0DE] = 0x2590;            // Right half block
////     lpMemRes[0x0DF] = 0x2580;            // Upper half block
////
////     lpMemRes[0x0E0] = UTF16_ALPHA;
////     lpMemRes[0x0E1] = 0x03B2;            // Beta
////     lpMemRes[0x0E2] = UTF16_LEFTSHOE;
////     lpMemRes[0x0E3] = UTF16_RIGHTSHOE;
////     lpMemRes[0x0E4] = UTF16_LAMP;
////     lpMemRes[0x0E5] = UTF16_UPCARETTILDE;
////     lpMemRes[0x0E6] = UTF16_RHO;
////     lpMemRes[0x0E7] = UTF16_DOWNCARETTILDE;
////     lpMemRes[0x0E8] = UTF16_CIRCLESTILE;
////     lpMemRes[0x0E9] = UTF16_CIRCLEBAR;
////     lpMemRes[0x0EA] = UTF16_CIRCLE;
////     lpMemRes[0x0EB] = UTF16_DOWNCARET;
////     lpMemRes[0x0EC] = UTF16_IOTA;
////     lpMemRes[0x0ED] = UTF16_CIRCLESLOPE;
////     lpMemRes[0x0EE] = UTF16_EPSILON;
////     lpMemRes[0x0EF] = UTF16_UPSHOE;
////
////     lpMemRes[0x0F0] = UTF16_SLASHBAR;
////     lpMemRes[0x0F1] = UTF16_SLOPEBAR;
////     lpMemRes[0x0F2] = UTF16_RIGHTCARETUNDERBAR;
////     lpMemRes[0x0F3] = UTF16_LEFTCARETUNDERBAR;
////     lpMemRes[0x0F4] = UTF16_NOTEQUAL;
////     lpMemRes[0x0F5] = UTF16_TIMES;
////     lpMemRes[0x0F6] = UTF16_COLONBAR;
////     lpMemRes[0x0F7] = UTF16_DELTAUNDERBAR;
////     lpMemRes[0x0F8] = UTF16_JOT;
////     lpMemRes[0x0F9] = UTF16_OMEGA;
////     lpMemRes[0x0FA] = UTF16_DELTILDE;
////     lpMemRes[0x0FB] = UTF16_DELTASTILE;
////     lpMemRes[0x0FC] = UTF16_DELSTILE;
////     lpMemRes[0x0FD] = UTF16_OVERBAR;
////     lpMemRes[0x0FE] = UTF16_DIERESIS;
////     lpMemRes[0x0FF] = 0x0000;            // ***Available***
////
////     lpMemRes[0x100] = 0x20AC;            // Euro
////     lpMemRes[0x101] = UTF16_CIRCLEMIDDLEDOT;
////     lpMemRes[0x102] = UTF16_DIERESISCIRCLE;
////     lpMemRes[0x103] = UTF16_DIERESISDEL;
////     lpMemRes[0x104] = UTF16_DIERESISDOWNTACK;
////     lpMemRes[0x105] = UTF16_DIERESISJOT;
////     lpMemRes[0x106] = UTF16_DIERESISSTAR;
////     lpMemRes[0x107] = UTF16_DIERESISTILDE;
////     lpMemRes[0x108] = UTF16_STILE;
////     lpMemRes[0x109] = UTF16_STILETILDE;
////     lpMemRes[0x10A] = UTF16_ZILDE;
////
//// #if QUADAV_LEN != (0x10A + 1)
//// #error Incorrect length for []AV
//// #endif
    // We no longer need this ptr
    MyGlobalUnlock (hGlbAV); lpMemRes = NULL;
} // End MakeQuadAV


//***************************************************************************
//  End of File: qf_av.c
//***************************************************************************
