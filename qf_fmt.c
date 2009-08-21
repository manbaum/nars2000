//***************************************************************************
//  NARS2000 -- System Function -- Quad FMT
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
#include "headers.h"
#include "nameclass.h"


//***************************************************************************
//  $SysFnFMT_EM_YY
//
//  System function:  []FMT -- Format
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnFMT_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnFMT_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonFMT_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydFMT_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnFMT_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonFMT_EM_YY
//
//  Monadic []FMT -- Boxed display
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonFMT_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonFMT_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    HGLOBAL      hGlbMF;            // Magic function ...
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
////HSHTABSTR    htsPTD;            // Old copy of HshTab struc
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    Assert (lptkAxis EQ NULL);

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get the magic function global memory handle
    hGlbMF = lpMemPTD->hGlbMF_MonFMT;

////// Save the old HshTab struc
////htsPTD = lpMemPTD->htsPTD;
////
////// Put the HshTab and SymTab into effect
////lpMemPTD->htsPTD = lpMemPTD->htsPTD_MonFMT;
////
////// Save address of previous struc
////lpMemPTD->htsPTD.lpHshTabPrv = &htsPTD;

    //  Return the array display
    //  Use an internal magic function.
    lpYYRes =
      ExecuteMagicFunction_EM_YY (NULL,         // Ptr to left arg token
                                  lptkFunc,     // Ptr to function token
                                  NULL,         // Ptr to function strand
                                  lptkRhtArg,   // Ptr to right arg token
                                  lptkAxis,     // Ptr to axis token
                                  hGlbMF,       // Magic function global memory handle
                                  LINENUM_ONE); // Starting line # type (see LINE_NUMS)
////// Delete address of previous struc
////lpMemPTD->htsPTD.lpHshTabPrv = NULL;
////
////// Copy back the contents of the current struc
////lpMemPTD->htsPTD_MonFMT = lpMemPTD->htsPTD;
////
////// Restore the old HTS
////lpMemPTD->htsPTD = htsPTD;

    return lpYYRes;
} // End SysFnMonFMT_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic function for Monadic []FMT
//
//  Monadic []FMT -- Boxed Display of Arrays
//
//  Return a boxed representation of an array.
//***************************************************************************

static APLCHAR MonHeaderFMT[] =
  L"Z" $IS MFN_MonFMT L" R;" $QUAD L"io;Type;Lft;Top;Proto;A";

static APLCHAR MonLineFMT1[] =
  $QUAD L"io" $IS L"0";

static APLCHAR MonLineFMT2[] =
  L":if 1<" $MATCH L"R";

static APLCHAR MonLineFMT3[] =
  L"  Type" $IS L"'" $EPSILON L"'";

static APLCHAR MonLineFMT4[] =
  L":else";

static APLCHAR MonLineFMT5[] =
  L"  :if 0" $EPSILON $RHO L"R";

static APLCHAR MonLineFMT6[] =
  L"    Proto" $IS $TAKE L"R";

static APLCHAR MonLineFMT7[] =
  L"  :else";

static APLCHAR MonLineFMT8[] =
  L"    Proto" $IS L"R";

static APLCHAR MonLineFMT9[] =
  L"  :endif";

static APLCHAR MonLineFMT10[] =
  L"  Type" $IS L"(0 ' '" $EPSILON $QUAD L"type Proto)/'~" $LDC_LT_HORZ $NEG L"'[0,1+0"
  $EPSILON $RHO L"R]";

static APLCHAR MonLineFMT11[] =
  L"  :select " $TIMES L"/" $RHO L"Type";

static APLCHAR MonLineFMT12[] =
  L"  :case 1";

static APLCHAR MonLineFMT13[] =
  L"    Type" $IS $ZILDE $RHO L"Type";

static APLCHAR MonLineFMT14[] =
  L"  :case 2";

static APLCHAR MonLineFMT15[] =
  L"    Type" $IS L"'+'";

static APLCHAR MonLineFMT16[] =
  L"  :endselect";

static APLCHAR MonLineFMT17[] =
  L":endif";

static APLCHAR MonLineFMT18[] =
  L"Proto" $IS L"0";

static APLCHAR MonLineFMT19[] =
  L":if 1" $GE $MATCH L"Z" $IS L"1/R";

static APLCHAR MonLineFMT20[] =
  L"  :if 0=" $RHO $RHO L"R";

static APLCHAR MonLineFMT21[] =
  L"    Z" $IS $FORMAT L"Z " $DIAMOND L" Z" $IS L"(1," $RHO L"Z)" $RHO L"Z";

static APLCHAR MonLineFMT22[] =
  L"    :if Type='" $LDC_LT_HORZ L"'";

static APLCHAR MonLineFMT23[] =
  L"      Z" $IS L"Z" $COMMABAR L"'" $NEG L"'";

static APLCHAR MonLineFMT24[] =
  L"    :elseif Type='~'";

static APLCHAR MonLineFMT25[] =
  L"      Z" $IS L"Z" $COMMABAR L"'~'";

static APLCHAR MonLineFMT26[] =
  L"    :endif";

static APLCHAR MonLineFMT27[] =
  L"    :return";

static APLCHAR MonLineFMT28[] =
  L"  :endif";

static APLCHAR MonLineFMT29[] =
  L"  Z" $IS L"((-2" $MAX $RHO $RHO L"Z)" $TAKE L"1," $RHO L"Z)" $RHO L"Z" $IS $FORMAT L"Z";

static APLCHAR MonLineFMT30[] =
  L"  :if 0" $EPSILON $RHO L"R";

static APLCHAR MonLineFMT31[] =
  L"    Z" $IS L"1 1" $RHO L"' 0'[Type='~']";

static APLCHAR MonLineFMT32[] =
  L"  :endif";

static APLCHAR MonLineFMT33[] =
  L"  Z" $IS L"Z " MFN_Box L" R";

static APLCHAR MonLineFMT34[] =
  L":else";

static APLCHAR MonLineFMT35[] =
  L"  :if 0" $EPSILON $RHO L"R";

static APLCHAR MonLineFMT36[] =
  L"    Z" $IS MFN_MonFMT $TAKE L"R";

static APLCHAR MonLineFMT37[] =
  L"    Z" $IS L"Z " MFN_Box L" R";

static APLCHAR MonLineFMT38[] =
  L"  :else";

static APLCHAR MonLineFMT39[] =
  L"    Z" $IS L"''";

static APLCHAR MonLineFMT40[] =
  L"    :forlcl I :in R";

static APLCHAR MonLineFMT41[] =
  L"      A" $IS MFN_MonFMT L" I";

static APLCHAR MonLineFMT42[] =
  L"      :if 0=" $MATCH L"I";

static APLCHAR MonLineFMT43[] =
  L"        A" $IS L"' '" $COMMABAR L"A";

static APLCHAR MonLineFMT44[] =
  L"      :endif";

static APLCHAR MonLineFMT45[] =
  L"      Z" $IS L"Z," $ENCLOSE L"A";

static APLCHAR MonLineFMT46[] =
  L"    :endforlcl";

static APLCHAR MonLineFMT47[] =
  L"    :if 1=" $RHO $RHO L"R";

static APLCHAR MonLineFMT48[] =
  L"      Z" $IS L"0 " $NEG L"1" $DROP $DISCLOSE L",/(" $EPSILON $MAX L"/" $RHO $EACH L"Z)[0]" $TAKE L"[0]" $EACH L"Z," $EACH L"' '";

static APLCHAR MonLineFMT49[] =
  L"    :else";

static APLCHAR MonLineFMT50[] =
  L"      Z" $IS $DISCLOSE L"(" $RHO L"R)" $RHO L"Z";

static APLCHAR MonLineFMT51[] =
  L"    :endif";

static APLCHAR MonLineFMT52[] =
  L"    Z" $IS L"((-3" $MAX $RHO $RHO L"Z)" $TAKE L"1," $RHO L"Z)" $RHO L"Z";

static APLCHAR MonLineFMT53[] =
  L"    Z" $IS L"(((" $NEG L"3" $DROP $RHO L"Z),(" $RHO L"Z)[" $NEG L"2],�/(" $RHO L"Z)[" $NEG L"3 " $NEG L"1])"
  $RHO L",[" $NEG L"2 " $NEG L"3] Z) " MFN_Box L" R";

static APLCHAR MonLineFMT54[] =
  L"  :endif";

static APLCHAR MonLineFMT55[] =
  L":endif";

static LPAPLCHAR MonBodyFMT[] =
{MonLineFMT1,
 MonLineFMT2,
 MonLineFMT3,
 MonLineFMT4,
 MonLineFMT5,
 MonLineFMT6,
 MonLineFMT7,
 MonLineFMT8,
 MonLineFMT9,
 MonLineFMT10,
 MonLineFMT11,
 MonLineFMT12,
 MonLineFMT13,
 MonLineFMT14,
 MonLineFMT15,
 MonLineFMT16,
 MonLineFMT17,
 MonLineFMT18,
 MonLineFMT19,
 MonLineFMT20,
 MonLineFMT21,
 MonLineFMT22,
 MonLineFMT23,
 MonLineFMT24,
 MonLineFMT25,
 MonLineFMT26,
 MonLineFMT27,
 MonLineFMT28,
 MonLineFMT29,
 MonLineFMT30,
 MonLineFMT31,
 MonLineFMT32,
 MonLineFMT33,
 MonLineFMT34,
 MonLineFMT35,
 MonLineFMT36,
 MonLineFMT37,
 MonLineFMT38,
 MonLineFMT39,
 MonLineFMT40,
 MonLineFMT41,
 MonLineFMT42,
 MonLineFMT43,
 MonLineFMT44,
 MonLineFMT45,
 MonLineFMT46,
 MonLineFMT47,
 MonLineFMT48,
 MonLineFMT49,
 MonLineFMT50,
 MonLineFMT51,
 MonLineFMT52,
 MonLineFMT53,
 MonLineFMT54,
 MonLineFMT55,
};

MAGIC_FUNCTION MF_MonFMT =
{MonHeaderFMT,
 MonBodyFMT,
 countof (MonBodyFMT),
};


//***************************************************************************
//  Magic function for Box
//
//  Box function -- Boxed Display For Arrays
//
//  Place line drawing chars around an array.
//***************************************************************************

static APLCHAR MonHeaderBox[] =
  L"Z" $IS L"Z " MFN_Box L" R;RD;LD;D";

static APLCHAR MonLineBox1[] =
  L"LD" $IS $FORMAT L"(0" $NOTEQUAL $RHO $RHO L"R)/" $NEG L"1" $TAKE $RHO L"R";

static APLCHAR MonLineBox2[] =
  L"RD" $IS L"0 1" $DROP $FORMAT $COMMABAR $NEG L"1" $DROP $RHO L"R";

static APLCHAR MonLineBox3[] =
  L"RD" $IS L"(+/RD=' ')" $ROTATE L"RD";

static APLCHAR MonLineBox4[] =
  L"D" $IS $NEG L"2+" $RHO $RHO L"Z";

static APLCHAR MonLineBox5[] =
  L":if D>0";

static APLCHAR MonLineBox6[] =
  L":while 2<" $RHO $RHO L"Z";

static APLCHAR MonLineBox7[] =
  L"  Z" $IS L",[" $NEG L"3 " $NEG L"2] Z,[" $NEG L"2]' '";

static APLCHAR MonLineBox8[] =
  L":endwhile";

static APLCHAR MonLineBox9[] =
  L"Z" $IS L"(-D)" $DROP L"[0]Z";

static APLCHAR MonLineBox10[] =
  L":endif";

static APLCHAR MonLineBox11[] =
  L"Z" $IS L"(" $NEG L"2" $TAKE L"1," $RHO L"Z)" $RHO L"Z " $DIAMOND
  L" Z" $IS L"((" $RHO L"Z)" $MAX L"(" $NEG L"1" $TAKE $RHO L"RD)," $RHO L"LD)" $TAKE L"Z";

static APLCHAR MonLineBox12[] =
  L":if 1" $GE $RHO $RHO L"R";

static APLCHAR MonLineBox13[] =
  L"  RD" $IS L"((1" $TAKE $RHO L"Z),1)" $RHO L"'" $LDC_LT_VERT L"'";

static APLCHAR MonLineBox14[] =
  L":else";

static APLCHAR MonLineBox15[] =
  L"  RD" $IS $TRANSPOSE L"(1" $TAKE $RHO L"Z)" $TAKE L"[1] RD " $DIAMOND
  L" RD[(,RD=' ')/," $IOTA $RHO L"RD]" $IS L"'" $LDC_LT_VERT L"'";

static APLCHAR MonLineBox16[] =
  L":endif";

static APLCHAR MonLineBox17[] =
  L"RD" $IS L"'" $LDC_LT_HORZ_D L"'" $COMMABAR L"RD" $COMMABAR L"'" $LDC_LT_HORZ_U L"' " $DIAMOND
  L" RD[0 " $NEG L"1;0]" $IS L"'" $LDC_LT_UL $LDC_LT_LL L"'";

static APLCHAR MonLineBox18[] =
  L"Z" $IS L"Z,'" $LDC_LT_VERT L"'";

static APLCHAR MonLineBox19[] =
  L"D" $IS $MATCH L"R";

static APLCHAR MonLineBox20[] =
  L":if 1<D";

static APLCHAR MonLineBox21[] =
  L"  D" $IS $FORMAT L"D";

static APLCHAR MonLineBox22[] =
  L"  Z[" $IOTA L"-" $RHO L"D;" $NEG L"1]" $IS $ROTATE L"D";

static APLCHAR MonLineBox23[] =
  L":endif";

static APLCHAR MonLineBox24[] =
  L"Z" $IS L"'" $LDC_LT_HORZ L"'" $COMMABAR L"Z" $COMMABAR L"'" $LDC_LT_HORZ L"'";

static APLCHAR MonLineBox25[] =
  L"Z[0 " $NEG L"1;" $NEG L"1]" $IS L"'" $LDC_LT_UR $LDC_LT_LR L"'";

static APLCHAR MonLineBox26[] =
  L"Z[0;" $IOTA $RHO L"LD]" $IS L"LD";

static APLCHAR MonLineBox27[] =
  L"Z[" $NEG L"1;0]" $IS L"Type";

static APLCHAR MonLineBox28[] =
  L"Z" $IS L"RD,Z";

static LPAPLCHAR MonBodyBox[] =
{MonLineBox1,
 MonLineBox2,
 MonLineBox3,
 MonLineBox4,
 MonLineBox5,
 MonLineBox6,
 MonLineBox7,
 MonLineBox8,
 MonLineBox9,
 MonLineBox10,
 MonLineBox11,
 MonLineBox12,
 MonLineBox13,
 MonLineBox14,
 MonLineBox15,
 MonLineBox16,
 MonLineBox17,
 MonLineBox18,
 MonLineBox19,
 MonLineBox20,
 MonLineBox21,
 MonLineBox22,
 MonLineBox23,
 MonLineBox24,
 MonLineBox25,
 MonLineBox26,
 MonLineBox27,
 MonLineBox28,
};

MAGIC_FUNCTION MF_Box =
{MonHeaderBox,
 MonBodyBox,
 countof (MonBodyBox),
};


//***************************************************************************
//  $SysFnDydFMT_EM_YY
//
//  Dyadic []FMT -- Format by specification
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydFMT_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydFMT_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    APLSTYPE       aplTypeLft,              // Left  arg storage type
                   aplTypeRht,              // Right ...
                   aplTypeItm;              // Item      ...
    APLNELM        aplNELMLft,              // Left  arg NELM
                   aplNELMRht,              // Right ...
                   aplNELMItm,              // Item      ...
                   aplColsRes,              // Result    cols
                   aplColsCur,              // Current   col #
                   aplColsRht,              // Right arg cols
                   aplColsItm;              // Item      cols
    APLRANK        aplRankLft,              // Left  arg rank
                   aplRankRht,              // Right ...
                   aplRankItm;              // Item      ...
    HGLOBAL        hGlbLft = NULL,          // Left arg global memory handle
                   hGlbRht = NULL,          // Right ...
                   hGlbItm,                 // Item     ...
                   hGlbRes = NULL;          // Result   ...
    LPAPLCHAR      lpMemLft = NULL,         // Ptr to left arg global memory
                   lpMemRes = NULL;         // ...    result   ...
    LPVOID         lpMemRht = NULL,         // Ptr to right arg global memory
                   lpMemItm;                // ...    item      ...
    LPPL_YYSTYPE   lpYYRes = NULL;          // Ptr to the result
    APLNELM        uCnt,                    // Loop counter
                   uLen,                    // Accumulated row product
                   aplNumCols,              // # accumulated cols in the right arg
                   aplNumRows,              // ...           rows ...
                   uFmtStr,                 // Loop counter
                   nCols;                   // Temporary column counter
    LPPERTABDATA   lpMemPTD;                // Ptr to PerTabData global memory
    LPFMTSPECSTR   lpfsInit,                // Ptr to initial FmtSpecStr output save area
                   lpfsNext,                // Ptr to next FmtSpecStr entry
                   lpfsCur,                 // Ptr to current FMTSPECSTR in a loop
                   lpfsEnd;                 // Ptr to end of the FMTSPECSTRs
    UINT           fsRep;                   // Iteration count
    APLUINT        ByteRes;                 // # bytes in the result
    UBOOL          bAbsorb = FALSE;         // TRUE iff there's at least one column-absorbing format spec
    APLLONGEST     aplLongestLft,           // Left arg as immediate value
                   aplLongestRht;           // Right ...
    APLDIM         aplCurColRes = 0;        // Current result col

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, &aplColsRht);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleChar (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Get left & right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the left arg is immediate, ...
    if (hGlbLft EQ NULL)
        lpMemLft = (LPAPLCHAR) &aplLongestLft;

    // If the right arg is immediate, ...
    if (hGlbRht EQ NULL)
        lpMemRht = &aplLongestRht;

    //  Allowed types are Simple, Hetero, nested depth <= 2.
    if (IsSimple (aplTypeRht))
    {
        // Count the # cols & rows in the right arg
        if (IsMultiRank (aplRankRht))
        {
            aplNumCols = aplColsRht;
            aplNumRows = 1;             // Initial value for loop
        } else
        {
            aplNumCols = 1;
            aplNumRows = aplColsRht;    // Initial value for loop
        } // End IF/ELSE

        // If the item is not immediate, ...
        if (hGlbRht)
        {
            // Skip over the header to the dimensions
            lpMemRht = VarArrayBaseToDim (lpMemRht);

            // Count the # rows in the right arg
            if (aplRankRht)
            {
                // Loop through all but the last col
                while (--aplRankRht)
                    aplNumRows *= *((LPAPLDIM) lpMemRht)++;

                // Skip over the last column
                ((LPAPLDIM) lpMemRht)++;
            } // End IF
        } // End IF

        // Now, lpMemRht points to the data
    } else
    {
        APLNELM aplNELMNst;

        // Check for RIGHT RANK ERROR
        if (IsMultiRank (aplRankRht))
            goto RIGHT_RANK_EXIT;

        // Ensure we count in the prototype if empty
        aplNELMNst = max (1, aplNELMRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // Loop through the right arg elements
        for (uCnt = aplNumCols = aplNumRows = 0; uCnt < aplNELMNst; uCnt++)
        // Split cases based upon the right arg item
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uCnt]))
        {
            case PTRTYPE_STCONST:
                // Accumulate the # cols in the right arg
                aplNumCols++;

                // Accumulate the # rows in the right arg
                aplNumRows = max (aplNumRows, 1);

                break;

            case PTRTYPE_HGLOBAL:
                // Get the global memory handle and
                //   clear the ptr type bits
                hGlbItm = ClrPtrTypeDir (((LPAPLNESTED) lpMemRht)[uCnt]);

                // Get the attributes (Type, NELM, and Rank)
                //   of the right arg item
                AttrsOfGlb (hGlbItm, &aplTypeItm, &aplNELMItm, &aplRankItm, &aplColsItm);

                // Check for RIGHT DOMAIN ERROR
                if (!IsSimple (aplTypeItm))
                    goto RIGHT_DOMAIN_EXIT;

                // Count the # cols & rows in the item
                if (IsMultiRank (aplRankItm))
                {
                    aplNumCols += aplColsItm;
                    uLen = 1;               // Initial value for loop
                } else
                {
                    aplNumCols++;
                    uLen = aplColsItm;      // Initial value for loop
                } // End IF/ELSE

                // Lock the memory to get a ptr to it
                lpMemItm = MyGlobalLock (hGlbItm);

                // Skip over the header to the dimensions
                lpMemItm = VarArrayBaseToDim (lpMemItm);

                // Count the # rows in the item
                if (aplRankItm)
                {
                    // Loop through all but the last col
                    while (--aplRankItm)
                        uLen *= *((LPAPLDIM) lpMemItm)++;

////////////////////// Skip over the last column
////////////////////((LPAPLDIM) lpMemItm)++;
                } // End IF

                // Accumulate the # rows in the right arg
                aplNumRows = max (aplNumRows, uLen);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbItm); lpMemItm = NULL;

                break;

            defstop
                break;
        } // End FOR/SWITCH
    } // End IF

    // Skip over the header and dimensions to the data
    if (hGlbLft)
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

    // At this point, the left arg is a simple char vector, and
    //  the right arg is either a simple array, or a nested vector
    //  of simple arrays.

    // Get ptr to format save area
    lpfsInit = lpfsNext = (LPFMTSPECSTR) lpMemPTD->lpwszTemp;

    // Trundle through the left arg validating it for syntax and
    //   accumulating information about the various fields
    if (!FmtSpec_EM (lpMemLft,      // Ptr to fmt spec
              (UINT) aplNELMLft,    // Actual length of lpwszLine
                     lpfsInit,      // Ptr to save area for fmt spec
                    &lpfsEnd,       // Ptr to ptr to end of the FMTSPECSTRs
                     lptkFunc))     // Ptr to function token
        goto ERROR_EXIT;

    // The format specification has been parsed and is summarized
    //   in lpfsInit.

    //***************************************************************
    //  Calculate the # cols in the result
    //***************************************************************

    // Loop through the right arg cols
    //   where a scalar/vector is treated as a one-column matrix
    for (uCnt = uFmtStr = aplColsRes = aplColsCur = 0,
           lpfsCur = lpfsInit,
           nCols = 1;
         (uCnt < aplNumCols)
      || (lpfsCur->fmtSpecVal EQ FMTSPECVAL_T)
      || (lpfsCur->fmtSpecVal EQ FMTSPECVAL_X)
      || (lpfsCur->fmtSpecVal EQ FMTSPECVAL_TXT);
        )
    {
        // Split cases based upon the format spec
        switch (lpfsCur->fmtSpecVal)
        {
            case FMTSPECVAL_A:
            case FMTSPECVAL_E:
            case FMTSPECVAL_F:
            case FMTSPECVAL_G:
            case FMTSPECVAL_I:
                // Calc the # remaining cols
                nCols = aplNumCols - uCnt;

                // Get the smaller of the # remaining cols and the repetition factor
                nCols = min (nCols, lpfsCur->fsRep);

                // Add up the # cols in the result
                aplColsCur += lpfsCur->fsWid * nCols;
                aplColsRes = max (aplColsRes, aplColsCur);

                // Mark as a column-absorbing fmtspec
                bAbsorb = TRUE;

                break;

            case FMTSPECVAL_T:
                // If the width is non-zero, ...
                if (lpfsCur->fsWid)
                {
                    // Set the current col to the value
                    aplColsCur = lpfsCur->fsWid - 1;
                    aplColsRes = max (aplColsRes, aplColsCur);
                } else
                    // Set the current col to the current maximum
                    aplColsCur = aplColsRes;
                // Don't use up a column of data
                nCols = 0;

                break;

            case FMTSPECVAL_X:
                // Get the repetition factor
                nCols = lpfsCur->fsRep;

                // If the width is non-negative, ...
                if (lpfsCur->fsWid >= 0)
                {
                    // Move the current col forward
                    aplColsCur += lpfsCur->fsWid * nCols;
                    aplColsRes = max (aplColsRes, aplColsCur);
                } else
                if (aplColsCur >= (-lpfsCur->fsWid * nCols))
                    // Move the current col backward
                    aplColsCur += lpfsCur->fsWid * nCols;
                else
                    goto FORMAT_EXIT;
                // Don't use up a column of data
                nCols = 0;

                break;

            case FMTSPECVAL_TXT:
                // Move the current col forward
                aplColsCur += lpfsCur->fsWid;
                aplColsRes = max (aplColsRes, aplColsCur);

                // Don't use up a column of data
                nCols = 0;

                break;

            defstop
                break;
        } // End SWITCH

        // Accumulate multiple cols
        uCnt += nCols;

        // Skip to the next FMTSPECSTR
        lpfsCur = (LPFMTSPECSTR) ByteAddr (lpfsCur, lpfsCur->offNext);

        // If the format spec is wrapping around, ...
        if (lpfsCur EQ lpfsEnd)
        {
            // If there are more data cols, wrap
            if (bAbsorb && uCnt < aplNumCols)
                // Restart the ptr
                lpfsCur = lpfsInit;
            else
                // Else quit
                break;
        } // End IF
    } // End FOR

    // Ensure there's at least one column-absorbing format spec
    //   or no columns
    if (!bAbsorb && aplNumCols NE 0)
        goto FORMAT_EXIT;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplColsRes * aplNumRows, 2);

    // Allocate space for the result
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////lpHeader->bSelSpec   = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplColsRes * aplNumRows;
    lpHeader->Rank       = 2;
#undef  lpHeader

    // Save the dimensions
    (VarArrayBaseToDim (lpMemRes))[0] = aplNumRows;
    (VarArrayBaseToDim (lpMemRes))[1] = aplColsRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 2);

    // Fill the result with blanks
    FillMemoryW (lpMemRes, (APLU3264) (aplColsRes * aplNumRows), L' ');

    // Protect the format save area
    lpMemPTD->lpwszTemp = (LPWCHAR) lpfsEnd;

    // Initialize the iteration count
    fsRep = lpfsInit->fsRep;

    // If the right arg is simple non-heterogeneous, ...
    if (IsSimpleNH (aplTypeRht))
        // Format the right arg into the result
        SysFnDydFMTSimp (aplTypeRht,            // Item storage type
                         aplNumCols,            // Item # cols
                         aplNumRows,            // Item # rows
                         aplColsRes,            // Result # cols
                         lpMemRht,              // Ptr to item global memory
                         lpfsInit,              // Ptr to first FMTSPECSTR
                        &lpfsNext,              // ...    ptr to next  ...
                         lpfsEnd,               // ...    end of the FMTSPECSTRs
                        &fsRep,                 // ...    iteration count
                         lpMemRes,              // Ptr to ptr to result global memory data
                        &aplCurColRes);         // Ptr to current result col
    else
    {
        LPSYMENTRY lpSymEntry;          // Ptr to STE

        // Loop through the right arg cols
        //   where a scalar/vector is treated as a one-row matrix
        for (uCnt = 0; uCnt < aplColsRht; uCnt++)
        // Split cases based upon the item ptr type
        switch (GetPtrTypeDir (((LPAPLNESTED) lpMemRht)[uCnt]))
        {
            case PTRTYPE_STCONST:
                // Get the STE ptr
                lpSymEntry = ((LPAPLHETERO) lpMemRht)[uCnt];

                // Get the item storage type
                aplTypeItm = TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType);

                // Get a ptr to the item value
                lpMemItm = &lpSymEntry->stData.stLongest;

                // Format the item into the result
                SysFnDydFMTSimp (aplTypeItm,            // Item storage type
                                 1,                     // Item # cols
                                 1,                     // Item # rows
                                 aplColsRes,            // Result # cols
                                 lpMemItm,              // Ptr to item global memory
                                 lpfsInit,              // Ptr to first FMTSPECSTR
                                &lpfsNext,              // ...    ptr to next  ...
                                 lpfsEnd,               // ...    end of the FMTSPECSTRs
                                &fsRep,                 // ...    iteration count
                                 lpMemRes,              // Ptr to ptr to result global memory data
                                &aplCurColRes);         // Ptr to current result col
                break;

            case PTRTYPE_HGLOBAL:
                // Get the global memory handle and
                //   clear the ptr type bits
                hGlbItm = ClrPtrTypeDir (((LPAPLNESTED) lpMemRht)[uCnt]);

                // Get the attributes (Type, NELM, and Rank)
                //   of the right arg item
                AttrsOfGlb (hGlbItm, &aplTypeItm, &aplNELMItm, &aplRankItm, &aplColsItm);

                // Count the # cols & rows in the item
                if (IsMultiRank (aplRankItm))
                {
                    aplNumCols = aplColsItm;
                    aplNumRows = 1;                     // Initial value for FOR loop
                } else
                {
                    aplNumCols = 1;
                    aplNumRows = aplColsItm;            // Initial value for FOR loop
                } // End IF/ELSE

                // Lock the memory to get a ptr to it
                lpMemItm = MyGlobalLock (hGlbItm);

                // Skip over the header to the dimensions
                lpMemItm = VarArrayBaseToDim (lpMemItm);

                // Count the # rows in the item
                if (aplRankItm)
                {
                    // Loop through all but the last col
                    while (--aplRankItm)
                        aplNumRows *= *((LPAPLDIM) lpMemItm)++;

                    // Skip over the last column
                    ((LPAPLDIM) lpMemItm)++;
                } // End IF

                // Now, lpMemItm points to the data

                // If the item is heterogeneous, ...
                if (IsSimpleHet (aplTypeItm))
                {
                    APLDIM       uCol,              // Loop counter
                                 uRow,              // ...
                                 aplCCR = 0;        // Temporary aplCurColRes
                    LPFMTSPECSTR lpfsCol;           // Ptr to FMTSPECSTR for this col
                    LPSYMENTRY   lpSymEntry;        // Ptr to STE
                    FMTSPECVAL   fmtSpecRes;        // Last FMTSPECVAL

                    // Initialize the FMTSPECSTR
                    lpfsCol = lpfsInit;

                    // Loop through the item cols
                    for (uCol = 0; uCol < aplNumCols; uCol++)
                    {
                        // Loop through the item rows
                        for (uRow = 0; uRow < aplNumRows; uRow++)
                        {
                            // Get a ptr to the STE
                            lpSymEntry = ((LPAPLHETERO) lpMemItm)[uRow *aplNumCols + uCol];

                            // Get the storage type
                            aplTypeItm = TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType);

                            // Calculate next offset within lpMemRes
                            aplCurColRes = aplCCR + uRow * aplColsRes;

                            // Respecify the ptr to the next FMTPSECSTR
                            lpfsNext = lpfsCol;

                            // Format the item into the result
                            fmtSpecRes =
                              SysFnDydFMTSimp (aplTypeItm,                      // Item storage type
                                               1,                               // Item # cols
                                               1,                               // Item # rows
                                               aplColsRes,                      // Result # cols
                                              &lpSymEntry->stData.stLongest,    // Ptr to item global memory
                                               lpfsInit,                        // Ptr to first FMTSPECSTR
                                              &lpfsNext,                        // ...    ptr to next  ...
                                               lpfsEnd,                         // ...    end of the FMTSPECSTRs
                                              &fsRep,                           // ...    iteration count
                                               lpMemRes,                        // Ptr to ptr to result global memory data
                                              &aplCurColRes);                   // Ptr to current result col
                        } // End FOR

                        // Save the last CurColRes for the next col
                        //   unless it's an absolute tab
                        if (fmtSpecRes NE FMTSPECVAL_T)
                            aplCCR = aplCurColRes - ((uRow - 1) * aplColsRes);

                        // Save the ptr to the next FMTSPECSTR for the next col
                        lpfsCol = lpfsNext;
                    } // End FOR
                } else
                    // Format the item into the result
                    SysFnDydFMTSimp (aplTypeItm,            // Item storage type
                                     aplNumCols,            // Item # cols
                                     aplNumRows,            // Item # rows
                                     aplColsRes,            // Result # cols
                                     lpMemItm,              // Ptr to item global memory
                                     lpfsInit,              // Ptr to first FMTSPECSTR
                                    &lpfsNext,              // ...    ptr to next  ...
                                     lpfsEnd,               // ...    end of the FMTSPECSTRs
                                    &fsRep,                 // ...    iteration count
                                     lpMemRes,              // Ptr to ptr to result global memory data
                                    &aplCurColRes);         // Ptr to current result col
                // We no longer need this ptr
                MyGlobalUnlock (hGlbItm); lpMemItm = NULL;

                break;

            defstop
                break;
        } // End FOR/SWITCH
    } // End IF/ELSE

    // Unprotect the format save area
    (LPFMTSPECSTR) lpMemPTD->lpwszTemp = lpfsInit;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

FORMAT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_FORMAT_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydFMT_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydFMTSimp
//
//  Format []FMT simple right arg
//
//  Returns the rightmost format spec
//***************************************************************************

FMTSPECVAL SysFnDydFMTSimp
    (APLSTYPE      aplTypeItm,              // Item storage type
     APLDIM        aplColsItm,              // Item # cols
     APLDIM        aplRowsItm,              // Item # rows
     APLDIM        aplColsRes,              // Result # cols
     LPVOID        lpMemItm,                // Ptr to item global memory
     LPFMTSPECSTR  lpfsIni,                 // Ptr to first FMTSPECSTR
     LPFMTSPECSTR *lplpfsNxt,               // ...    next  ...
     LPFMTSPECSTR  lpfsEnd,                 // ...    end of the FMTSPECSTRs
     LPUINT        lpfsRep,                 // ...    iteration count
     LPAPLCHAR     lpMemRes,                // Ptr to ptr to result global memory data
     LPAPLDIM      lpaplCurColRes)          // Current result column

{
    APLDIM       uRow,                      // Loop counter
                 aplCurColItm,              // Current item column in the current row
                 aplCurColRes,              // ...     result ...
                 aplMaxColRes;              // Maximum result ...
    UINT         fsRep,                     // Repetition factor
                 fsDig,                     // Precision for F-format, significant digits for E-format
                 uRep;                      // Loop counter
    int          fsWid;                     // Field width
    LPWCHAR      lpwSub,                    // Ptr to temporary Symbol Substitution
                 lpwSrc;                    // Ptr to source text
    WCHAR        wch;                       // ...       WCHAR
    LPAPLCHAR    lpwszFormat;               // Ptr to formatting save area
    LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory
    SURRTEXT     surrText;                  // Surrounding Text info
    FMTSPECVAL   fmtSpecRes;                // Result

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // If the format spec is wrapping around, ...
    if (*lplpfsNxt EQ lpfsEnd)
        *lplpfsNxt = lpfsIni;

    // If there are any cols, ...
    if (aplColsItm)
        // Split cases based upon the item storage type
        switch (aplTypeItm)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
                // Loop through the item cols
                //   where a scalar/vector is treated as a one-column matrix
                for (aplCurColItm = 0, aplMaxColRes = aplCurColRes = *lpaplCurColRes;
                     (aplCurColItm < aplColsItm)
                  || ((*lplpfsNxt)->fmtSpecVal EQ FMTSPECVAL_T)
                  || ((*lplpfsNxt)->fmtSpecVal EQ FMTSPECVAL_X)
                  || ((*lplpfsNxt)->fmtSpecVal EQ FMTSPECVAL_TXT);
                     )
                {
                    // Save the value of the last FMTSPECVAL
                    fmtSpecRes = (*lplpfsNxt)->fmtSpecVal;

                    // If the residual iteration count is zero, ...
                    if (*lpfsRep EQ 0)
                        // Get the repetition factor, width, and precision
                        fsRep = *lpfsRep = (*lplpfsNxt)->fsRep;
                    else
                        fsRep = *lpfsRep;

                    // If the format string admits a repetition count, ...
                    if (fmtSpecRes NE FMTSPECVAL_T
                     && fmtSpecRes NE FMTSPECVAL_X
                     && fmtSpecRes NE FMTSPECVAL_TXT)
                        // Use the smaller of the # remaining cols and the count
                        fsRep = min (fsRep, (UINT) (aplColsItm - aplCurColItm));
                    fsWid = (*lplpfsNxt)->fsWid;
                    fsDig = (*lplpfsNxt)->fsDig;

                    // If there's an S-qualifier, ...
                    if ((*lplpfsNxt)->bS)
                        lpwSub = (LPWCHAR) ByteAddr (*lplpfsNxt, (*lplpfsNxt)->offStxt);
                    else
                        lpwSub = STDSYMSUB;

                    // Loop through the repetition factor
                    for (uRep = 0; uRep < fsRep; uRep++, aplCurColItm++)
                    {
                        // Split cases based upon the format spec
                        switch (fmtSpecRes)
                        {
                            case FMTSPECVAL_A:
                                // Loop through the item rows
                                for (uRow = 0; uRow < aplRowsItm; uRow++)
                                    // Overflow
                                    FillMemoryW (&lpMemRes[(uRow * aplColsRes) + aplCurColRes], fsWid, lpwSub[SYMSUB_OVERFLOW_FILL]);
                                break;

                            case FMTSPECVAL_G:
                                // Loop through the item rows
                                for (uRow = 0; uRow < aplRowsItm; uRow++)
                                    // Continue with code common to G-format spec
                                    QFMT_CommonG (*lplpfsNxt,                                       // Ptr to FMTSPECSTR
                                                   aplTypeItm,                                      // Item storage type
                                                   fmtSpecRes,                                      // Result format spec
                                                   lpMemItm,                                        // Ptr to item global memory
                                                   uRow,                                            // Row #
                                                   aplColsItm,                                      // Item # cols
                                                   aplColsRes,                                      // Result # cols
                                                   aplCurColItm,                                    // Current item col in current row
                                                   aplCurColRes,                                    // ...     result ...
                                                   lpwszFormat,                                     // Ptr to formatted item
                                                   lpwSub,                                          // Ptr to Symbol Substitution
                                                  &lpMemRes[(uRow * aplColsRes) + aplCurColRes],    // Ptr to result global memory
                                                  &surrText);                                       // Ptr to Surrounding Text info
                                break;

                            case FMTSPECVAL_E:
                            case FMTSPECVAL_F:
                            case FMTSPECVAL_I:
                                // Loop through the item rows
                                for (uRow = 0; uRow < aplRowsItm; uRow++)
                                    // Continue with code common to E- F- I-format specs
                                    QFMT_CommonEFI (*lplpfsNxt,                                     // Ptr to FMTSPECSTR
                                                     aplTypeItm,                                    // Item storage type
                                                     fmtSpecRes,                                    // Result format spec
                                                     lpMemItm,                                      // Ptr to item global memory
                                                     uRow,                                          // Row #
                                                     aplColsItm,                                    // Item # cols
                                                     aplColsRes,                                    // Result # cols
                                                     aplCurColItm,                                  // Current item col in current row
                                                     aplCurColRes,                                  // ...     result ...
                                                     lpwszFormat,                                   // Ptr to formatted item
                                                     lpwSub,                                        // Ptr to Symbol Substitution
                                                    &lpMemRes[(uRow * aplColsRes) + aplCurColRes],  // Ptr to result global memory
                                                    &surrText);                                     // Ptr to Surrounding Text info
                                break;

                            case FMTSPECVAL_T:
                                // If the value is to an absolute position, ...
                                if (fsWid)
                                    // Save the absolute position
                                    *lpaplCurColRes = fsWid - 1;
                                else
                                    // Save the rightmost position
                                    *lpaplCurColRes = aplMaxColRes;
                                aplCurColRes = *lpaplCurColRes;

                                // Save maximum position
                                aplMaxColRes = max (aplMaxColRes, aplCurColRes);

                                // Don't use up a column of data
                                aplCurColItm--;

                                // Don't skip over the text
                                continue;

                            case FMTSPECVAL_X:
                                // If it's non-negative, ...
                                if (fsWid >= 0 || aplCurColRes >= -fsWid)
                                {
                                    *lpaplCurColRes += fsWid;
                                    aplCurColRes = *lpaplCurColRes;
                                } // End IF

                                // Save maximum position
                                aplMaxColRes = max (aplMaxColRes, aplCurColRes);

                                // Don't use up a column of data
                                aplCurColItm--;

                                // Don't skip over the text
                                continue;

                            case FMTSPECVAL_TXT:
                                // Get ptr to source text and width
                                lpwSrc = (LPWCHAR) ByteAddr (*lplpfsNxt, (*lplpfsNxt)->offText);

                                // Loop through the item rows
                                for (uRow = 0; uRow < aplRowsItm; uRow++)
                                    CopyMemoryW (&lpMemRes[(uRow * aplColsRes) + aplCurColRes],
                                                  lpwSrc,
                                                  fsWid);
                                // Don't use up a column of data
                                aplCurColItm--;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // Skip over the text
                        *lpaplCurColRes += fsWid;
                        aplCurColRes = *lpaplCurColRes;

                        // Save maximum position
                        aplMaxColRes = max (aplMaxColRes, aplCurColRes);
                    } // End FOR

                    // Less the current repetitions
                    *lpfsRep -= fsRep;

                    // If there are more repetitions, don't skip to the next FMTSPECSTR
                    if (*lpfsRep)
                        break;      // Out of the <aplColsItm> FOR loop

                    // Skip to the next format spec
                    *lplpfsNxt = (LPFMTSPECSTR) ByteAddr (*lplpfsNxt, (*lplpfsNxt)->offNext);

                    // If the format spec is wrapping around, ...
                    if (*lplpfsNxt EQ lpfsEnd)
                    {
                        // If there are more data cols, wrap
                        if (aplCurColItm < aplColsItm)
                            // Restart the ptr
                            *lplpfsNxt = lpfsIni;
                        else
                            // Else quit
                            break;

                        // Save the current repetitions
                        *lpfsRep = (*lplpfsNxt)->fsRep;
                    } // End IF
                } // End FOR

                break;

            case ARRAY_CHAR:
                // Loop through the item cols
                for (aplCurColItm = 0, aplCurColRes = *lpaplCurColRes;
                     (aplCurColItm < aplColsItm)
                  || ((*lplpfsNxt)->fmtSpecVal EQ FMTSPECVAL_T)
                  || ((*lplpfsNxt)->fmtSpecVal EQ FMTSPECVAL_X)
                  || ((*lplpfsNxt)->fmtSpecVal EQ FMTSPECVAL_TXT);
                     )
                {
                    // Save the value of the last FMTSPECVAL
                    fmtSpecRes = (*lplpfsNxt)->fmtSpecVal;

                    // If the residual iteration count is zero, ...
                    if (*lpfsRep EQ 0)
                        // Get the repetition factor, width, and precision
                        fsRep = *lpfsRep = (*lplpfsNxt)->fsRep;
                    else
                        fsRep = *lpfsRep;

                    // If the format string admits a repetition count, ...
                    if (fmtSpecRes NE FMTSPECVAL_T
                     && fmtSpecRes NE FMTSPECVAL_X
                     && fmtSpecRes NE FMTSPECVAL_TXT)
                        // Use the smaller of the # remaining cols and the count
                        fsRep = min (fsRep, (UINT) (aplColsItm - aplCurColItm));
                    fsWid = (*lplpfsNxt)->fsWid;
////////////////////fsDig = (*lplpfsNxt)->fsDig;        // Unused for char data

                    // If there's an S-qualifier, ...
                    if ((*lplpfsNxt)->bS)
                        lpwSub = (LPWCHAR) ByteAddr (*lplpfsNxt, (*lplpfsNxt)->offStxt);
                    else
                        lpwSub = STDSYMSUB;

                    // Loop through the repetition factor
                    for (uRep = 0; uRep < fsRep; uRep++, aplCurColItm++)
                    {
                        // Split cases based upon the format spec
                        switch (fmtSpecRes)
                        {
                            case FMTSPECVAL_A:
                                // If the width is non-zero, ...
                                if (fsWid)
                                {
                                    // Loop through the item rows
                                    for (uRow = 0; uRow < aplRowsItm; uRow++)
                                    {
                                        // Fill in the background if present (R-modifier)
                                        if ((*lplpfsNxt)->bR)
                                            QFMT_FillBg (*lplpfsNxt, &lpMemRes[(uRow * aplColsRes) + aplCurColRes]);

                                        // Get the char to save
                                        wch = ((LPAPLCHAR) lpMemItm)[(uRow * aplColsItm) + aplCurColItm];

                                        // If the text is to be left-justified, ...
                                        if ((*lplpfsNxt)->bL)
                                            lpMemRes[(uRow * aplColsRes) + aplCurColRes]             = wch;
                                        else
                                            lpMemRes[(uRow * aplColsRes) + aplCurColRes + fsWid - 1] = wch;
                                    } // End FOR
                                } // End IF

                                break;

                            case FMTSPECVAL_E:
                            case FMTSPECVAL_F:
                            case FMTSPECVAL_G:
                            case FMTSPECVAL_I:
                                // Loop through the item rows
                                for (uRow = 0; uRow < aplRowsItm; uRow++)
                                    // Overflow
                                    FillMemoryW (&lpMemRes[(uRow * aplColsRes) + aplCurColRes], fsWid, lpwSub[SYMSUB_OVERFLOW_FILL]);
                                break;

                            case FMTSPECVAL_T:
                                // If the value is to an absolute position, ...
                                if (fsWid)
                                    // Save the absolute position
                                    *lpaplCurColRes = fsWid - 1;
                                else
                                    // Save the rightmost position
                                    *lpaplCurColRes = aplMaxColRes;
                                aplCurColRes = *lpaplCurColRes;

                                // Save maximum position
                                aplMaxColRes = max (aplMaxColRes, aplCurColRes);

                                // Don't use up a column of data
                                aplCurColItm--;

                                // Don't skip over the text
                                continue;

                            case FMTSPECVAL_X:
                                // If it's non-negative, ...
                                if (fsWid >= 0 || aplCurColRes >= -fsWid)
                                {
                                    *lpaplCurColRes += fsWid;
                                    aplCurColRes = *lpaplCurColRes;
                                } // End IF

                                // Save maximum position
                                aplMaxColRes = max (aplMaxColRes, aplCurColRes);

                                // Don't use up a column of data
                                aplCurColItm--;

                                // Don't skip over the text
                                continue;

                            case FMTSPECVAL_TXT:
                                // Get ptr to source text
                                lpwSrc = (LPWCHAR) ByteAddr (*lplpfsNxt, (*lplpfsNxt)->offText);

                                // Loop through the item rows
                                for (uRow = 0; uRow < aplRowsItm; uRow++)
                                    CopyMemoryW (&lpMemRes[(uRow * aplColsRes) + aplCurColRes],
                                                  lpwSrc,
                                                  fsWid);
                                // Don't use up a column of data
                                aplCurColItm--;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // Skip over the text
                        *lpaplCurColRes += fsWid;
                        aplCurColRes = *lpaplCurColRes;

                        // Save maximum position
                        aplMaxColRes = max (aplMaxColRes, aplCurColRes);
                    } // End FOR

                    // Less the current repetitions
                    *lpfsRep -= fsRep;

                    // If there are more repetitions, don't skip to the next FMTSPECSTR
                    if (*lpfsRep)
                        break;      // Out of the <aplColsItm> FOR loop

                    // Skip to the next format spec
                    *lplpfsNxt = (LPFMTSPECSTR) ByteAddr (*lplpfsNxt, (*lplpfsNxt)->offNext);

                    // If the format spec is wrapping around, ...
                    if (*lplpfsNxt EQ lpfsEnd)
                    {
                        // If there are more data cols, quit
                        if (aplCurColItm < aplColsItm)
                            // Restart the ptr
                            *lplpfsNxt = lpfsIni;
                        else
                            // Else quit
                            break;

                        // Save the current repetitions
                        *lpfsRep = (*lplpfsNxt)->fsRep;
                    } // End IF
                } // End FOR

                break;

            case ARRAY_HETERO:          // It's a simple array
            case ARRAY_NESTED:          // ...
            defstop
                break;
        } // End SWITCH
    else
        // Initialize the return value
        fmtSpecRes = FMTSPECVAL_ERR;

    return fmtSpecRes;
} // End SysFnDydFMTSimp


//***************************************************************************
//  $QFMT_CommonG
//
//  Code common to G-format spec
//***************************************************************************

void QFMT_CommonG
    (LPFMTSPECSTR lpfsNxt,                  // Ptr to FMTSPECSTR
     APLSTYPE     aplTypeItm,               // Item storage type
     FMTSPECVAL   fmtSpecRes,               // Result format spec
     LPVOID       lpMemItm,                 // Ptr to item global memory
     APLDIM       uRow,                     // Row #
     APLDIM       aplColsItm,               // Item # cols
     APLDIM       aplColsRes,               // Result # cols
     APLDIM       aplCurColItm,             // Current item column in the current row
     APLDIM       aplCurColRes,             // ...     result ...
     LPAPLCHAR    lpwszFormat,              // Ptr to formatted item
     LPAPLCHAR    lpwSub,                   // Ptr to Symbol Substitution
     LPAPLCHAR    lpMemRes,                 // Ptr to result global memory
     LPSURRTEXT   lpSurrText)               // Ptr to Surrounding Text info

{
    APLLONGEST   aplLongestItm;             // Next value as integer or float
    IMM_TYPES    immTypeItm;                // Immediate type of above value
    APLINT       aplIntItm;                 // Next item as an integer
    APLFLOAT     aplFltItm;                 // ...             float
    LPAPLCHAR    lpaplChar;                 // Ptr to next available byte
    int          fsWid;                     // Field width
    UBOOL        bDigSel,                   // TRUE iff a digit was selected last
                 bTrlTxt,                   // TRUE iff nothing in result but trailing text so far
                 bNeg;                      // TRUE iff the formatted number is negative
    APLCHAR      aplChar9,                  // 9 digit selector
                 aplCharZ;                  // Z ...
    LPAPLCHAR    lpwPat,                    // Ptr to G-format pattern
                 lpwNxt,                    // Loop counter
                 lpwDst,                    // Ptr to end of lpMemRes
                 lpwLeftDig;                // Ptr to leftmost digit in the result

    // Modifiers allowed for G-format:  B   K   M   O P   R S   Rep
    // ...       not                 :    C   L   N     Q     Z

    // Initialize variables
    fsWid = lpfsNxt->fsWid;
    aplChar9 = lpwSub[SYMSUB_9_CHAR];
    aplCharZ = lpwSub[SYMSUB_Z_CHAR];

    // Clear the Surrounding Text struc
    ZeroMemory (lpSurrText, sizeof (lpSurrText[0]));

    // Get the next value
    GetNextValueMem (lpMemItm,                              // Ptr to item global memory data
                     aplTypeItm,                            // Item storage type
                     (uRow * aplColsItm) + aplCurColItm,    // Index into item
                     NULL,                                  // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    &aplLongestItm,                         // Ptr to result immediate value (may be NULL)
                    &immTypeItm);                           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // Fill in the background if present (R-modifier)
    if ((lpfsNxt)->bR)
        QFMT_FillBg (lpfsNxt, lpMemRes);

    // Get the value as an integer/float
    if (IsImmInt (immTypeItm))
    {
        aplIntItm = (APLINT) aplLongestItm;
        aplFltItm = (APLFLOAT) aplIntItm;
    } else
    {
        aplIntItm = (APLINT) *(LPAPLFLOAT) &aplLongestItm;
        aplFltItm = *(LPAPLFLOAT) &aplLongestItm;
    } // End IF/ELSE

    // Compare value and substitute text (O-modifier)
    if (lpfsNxt->bO
     && QFMT_SubstValue (lpfsNxt,                           // Ptr to FMTSPECSTR
                         immTypeItm,                        // Item immediate type
                         lpMemItm,                          // Ptr to item global memory
                         aplIntItm,                         // Next value as an integer
                         aplFltItm,                         // ...              float
                         lpMemRes))                         // Ptr to result global memory
        // Move to the next row
        return;

    // Blank if zero (B-modifier)
    if (lpfsNxt->bB && aplIntItm EQ 0)
        // Move to the next row
        return;

    // Scale the value (K-modifier)
    if (lpfsNxt->bK)
    {
        int fsScl;

        // Get the scaling factor
        fsScl = lpfsNxt->fsScl;

        // Split cases based upon the item immediate type
        switch (immTypeItm)
        {
            case IMMTYPE_BOOL:
            case IMMTYPE_INT:
                if (fsScl < 0)
                    while (fsScl++)
                        aplIntItm /= 10;
                else
                    while (fsScl--)
                        aplIntItm *= 10;
                break;

            case IMMTYPE_FLOAT:
                // If the scaling factor is negative, ...
                if (fsScl < 0)
                    while (fsScl++)
                        aplFltItm /= 10;
                else
                    while (fsScl--)
                        aplFltItm *= 10;
                break;

            case IMMTYPE_CHAR:
            defstop
                break;
        } // End SWITCH
    } // End IF

    // Format the value
    if (IsImmInt (immTypeItm))
        lpaplChar =
          FormatAplintFC (lpwszFormat,                  // Ptr to output save area
                          aplIntItm,                    // The value to format
                          UTF16_OVERBAR);               // Char to use as overbar
    else
        lpaplChar =
          FormatFloatFC (lpwszFormat,                   // Ptr to output save area
                         aplFltItm,                     // The value to format
                         fsWid,                         // Precision for F-format, significant digits for E-format
                         UTF16_DOT,                     // Char to use as decimal separator
                         UTF16_OVERBAR,                 // Char to use as overbar
                         FLTDISPFMT_RAWINT);            // Float display format
    // Ensure properly terminated
    *--lpaplChar = WC_EOS;

    // Calculate length of and ptr to negative text to prepend (M-modifier)
    if (lpfsNxt->bM && aplFltItm < 0)
    {
        // Get ptr to M-modifier text
        lpSurrText->lpHead = (LPAPLCHAR) ByteAddr (lpfsNxt, lpfsNxt->offMtxt);

        // Get the M-modifier text length
        lpSurrText->uHead = lstrlenW (lpSurrText->lpHead);
    } // End IF

    // Calculate length of and ptr to positive text to prepend (P-modifier)
    if (lpfsNxt->bP && aplFltItm > 0)
    {
        // Get ptr to P-modifier text
        lpSurrText->lpHead = (LPAPLCHAR) ByteAddr (lpfsNxt, lpfsNxt->offPtxt);

        // Get the M-modifier text length
        lpSurrText->uHead = lstrlenW (lpSurrText->lpHead);
    } // End IF

    // Get ptr to start and end of G-modifier text
    lpwPat = (LPAPLCHAR) ByteAddr (lpfsNxt, lpfsNxt->offGtxt);
    lpwNxt = &lpwPat[fsWid];

    // Get ptr to end of result
    lpwDst = &lpMemRes[fsWid];

    // Initialize digit selector flag
    bDigSel = FALSE;

    // Initialize initial trailing text flag
    bTrlTxt = TRUE;

    // Check for negatives
    bNeg = (lpwszFormat[0] EQ UTF16_OVERBAR);

    // Loop through the pattern from right to left copying
    //   text or digits to the result
    while (lpwPat <= --lpwNxt)
    if (*lpwNxt EQ aplChar9)
    {
        if (&lpwszFormat[bNeg] < lpaplChar)
            *--lpwDst = *--lpaplChar;
        else
            *--lpwDst = L'0';
        bDigSel = TRUE;

        // Save ptr to leftmost digit
        lpwLeftDig = lpwDst;

        // No more trailing text
        bTrlTxt = FALSE;
    } else
    if (*lpwNxt EQ aplCharZ)
    {
        if (&lpwszFormat[bNeg] < lpaplChar)
        {
            *--lpwDst = *--lpaplChar;

            if (!bDigSel && lpaplChar[0] EQ L'0')
                lpwDst[0] = L' ';
            else
                // Save ptr to leftmost digit
                lpwLeftDig = lpwDst;

            bDigSel = (lpwDst[0] NE L' ');
        } else
        {
            --lpwDst;
            bDigSel = FALSE;
        } // End IF/ELSE

        // No more trailing text
        bTrlTxt = FALSE;
    } else
    if (bTrlTxt
     || (bDigSel
      && &lpwszFormat[bNeg] < lpaplChar))
        *--lpwDst = *lpwNxt;
    else
        --lpwDst;

    // If there are more digits in the formatted number, ...
    if (&lpwszFormat[bNeg] < lpaplChar)
    {
        // Insert overflow chars
        FillMemoryW (lpMemRes, fsWid, lpwSub[SYMSUB_OVERFLOW_FILL]);

        return;
    } // End IF

    if (bNeg)
    {
        // If there's room for a negative sign, ...
        if (lpMemRes < lpwLeftDig)
        {
            // Skip this is we're inserting M-modifier text
            if (!(lpfsNxt->bM && aplFltItm < 0))
                *--lpwLeftDig = UTF16_OVERBAR;
        } else
        {
            // Insert overflow chars
            FillMemoryW (lpMemRes, fsWid, lpwSub[SYMSUB_OVERFLOW_FILL]);

            return;
        } // End IF/ELSE
    } // End IF

    // Get ptr to end of the pattern
    lpwNxt = &lpwPat[fsWid];
    lpwDst = lpMemRes;

    // In order to copy the leading non-9Z text,
    //   we need to run through the pattern once again
    while (lpwPat < lpwNxt)
    if (aplChar9 EQ *lpwPat
     || aplCharZ EQ *lpwPat)
        break;
    else
        *lpwDst++ = *lpwPat++;

    // Prepend with negative text (M-modifier)
    // Prepend with positive text (P-modifier)
    if ((lpfsNxt->bM && aplFltItm < 0)
     || (lpfsNxt->bP && aplFltItm > 0))
    {
        APLU3264 uLen;

        // Calculate the amount of room before the leftmost digits
        uLen = (lpwLeftDig - lpMemRes);

        // If there's room for the leading text, ...
        if (uLen >= lpSurrText->uHead)
            // Copy the M- or P-modifier text
            CopyMemoryW (&lpMemRes[uLen - lpSurrText->uHead], lpSurrText->lpHead, lpSurrText->uHead);
        else
            // Insert overflow chars
            FillMemoryW (lpMemRes, fsWid, lpwSub[SYMSUB_OVERFLOW_FILL]);
    } // End IF
} // End QFMT_CommonG


//***************************************************************************
//  $QFMT_CommonEFI
//
//  Code common to E- F- and I-format specs
//***************************************************************************

void QFMT_CommonEFI
    (LPFMTSPECSTR lpfsNxt,                  // Ptr to FMTSPECSTR
     APLSTYPE     aplTypeItm,               // Item storage type
     FMTSPECVAL   fmtSpecRes,               // Result format spec
     LPVOID       lpMemItm,                 // Ptr to item global memory
     APLDIM       uRow,                     // Row #
     APLDIM       aplColsItm,               // Item # cols
     APLDIM       aplColsRes,               // Result # cols
     APLDIM       aplCurColItm,             // Current item column in the current row
     APLDIM       aplCurColRes,             // ...     result ...
     LPAPLCHAR    lpwszFormat,              // Ptr to formatted item
     LPAPLCHAR    lpwSub,                   // Ptr to Symbol Substitution
     LPAPLCHAR    lpMemRes,                 // Ptr to result global memory
     LPSURRTEXT   lpSurrText)               // Ptr to Surrounding Text info

{
    APLLONGEST   aplLongestItm;             // Next value as integer or float
    IMM_TYPES    immTypeItm;                // Immediate type of above value
    APLINT       aplIntItm;                 // Next item as an integer
    APLFLOAT     aplFltItm;                 // ...             float
    LPAPLCHAR    lpaplChar;                 // Ptr to next available byte
    int          fsWid,                     // Field width
                 iWid;                      // ...
    APLU3264     fsDig;                     // Precision for F-format, significant digits for E-format
    UBOOL        bContinue,                 // TRUE iff we're to continue with Symbol Substitution
                 bZeroFill;                 // TRUE iff we're still doing Zero Fill
    LPWCHAR      lpwSrc,                    // Ptr to source text
                 lpwEnd,                    // Ptr to end of ...
                 lpwSymChar;                // Ptr to symbol to substitute

    // Modifiers allowed for E-format:  B   K L M N O P Q R S Z Rep
    // ...       not ...             :    C
    // Modifiers allowed for F-format:  all
    // ...       not ...             :  none
    // Modifiers allowed for I-format:  all
    // ...       not ...             :  none

    // Initialize variables
    fsWid = lpfsNxt->fsWid;
    fsDig = lpfsNxt->fsDig;

    // Clear the Surrounding Text struc
    ZeroMemory (lpSurrText, sizeof (lpSurrText[0]));

    // Get the next value
    GetNextValueMem (lpMemItm,                              // Ptr to item global memory data
                     aplTypeItm,                            // Item storage type
                     (uRow * aplColsItm) + aplCurColItm,    // Index into item
                     NULL,                                  // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                    &aplLongestItm,                         // Ptr to result immediate value (may be NULL)
                    &immTypeItm);                           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
    // Fill in the background if present (R-modifier)
    if ((lpfsNxt)->bR)
        QFMT_FillBg (lpfsNxt, lpMemRes);

    // Get the value as an integer/float
    if (IsImmInt (immTypeItm))
    {
        aplIntItm = (APLINT) aplLongestItm;
        aplFltItm = (APLFLOAT) aplIntItm;
    } else
    {
        aplIntItm = (APLINT) *(LPAPLFLOAT) &aplLongestItm;
        aplFltItm = *(LPAPLFLOAT) &aplLongestItm;
    } // End IF/ELSE

    // Compare value and substitute text (O-modifier)
    if (lpfsNxt->bO
     && QFMT_SubstValue (lpfsNxt,                           // Ptr to FMTSPECSTR
                         immTypeItm,                        // Item immediate type
                         lpMemItm,                          // Ptr to item global memory
                         aplIntItm,                         // Next value as an integer
                         aplFltItm,                         // ...              float
                         lpMemRes))                         // Ptr to result global memory
        // Move to the next row
        return;

    // Blank if zero (B-modifier)
    if (lpfsNxt->bB && aplIntItm EQ 0)
        // Move to the next row
        return;

    // Scale the value (K-modifier)
    if (lpfsNxt->bK)
    {
        int fsScl;

        // Get the scaling factor
        fsScl = lpfsNxt->fsScl;

        // Split cases based upon the format spec value
        switch (fmtSpecRes)
        {
            case FMTSPECVAL_E:
            case FMTSPECVAL_F:
                // If the scaling factor is negative, ...
                if (fsScl < 0)
                    while (fsScl++)
                        aplFltItm /= 10;
                else
                    while (fsScl--)
                        aplFltItm *= 10;
                break;

            case FMTSPECVAL_I:
                if (fsScl < 0)
                    while (fsScl++)
                        aplIntItm /= 10;
                else
                    while (fsScl--)
                        aplIntItm *= 10;
                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    // Format the value
    // Split cases based upon the format spec value
    switch (fmtSpecRes)
    {
        case FMTSPECVAL_E:
        {
            int iExpCnt;                                // Loop counter

            lpwEnd =
              FormatFloatFC (lpwszFormat,               // Ptr to output save area
                             aplFltItm,                 // The value to format
                             fsDig,                     // Precision for F-format, significant digits for E-format
                             UTF16_DOT,                 // Char to use as decimal separator
                             UTF16_OVERBAR,             // Char to use as overbar
                             FLTDISPFMT_E);             // Float display format
            // Append enough blanks to the result to fill
            //   out the exponent to full width ("E-100")
            iExpCnt = 5 - ((UINT) (lpwEnd - strchrW (lpwszFormat, L'E')) - 1);
            while (iExpCnt-- > 0)
                *lpwEnd++ = L' ';

            // Ensure properly terminated
            *--lpwEnd = WC_EOS;

            break;
        } // End FMTSPECVAL_E

        case FMTSPECVAL_F:
            lpwEnd =
              FormatFloatFC (lpwszFormat,               // Ptr to output save area
                             aplFltItm,                 // The value to format
                             fsDig,                     // Precision for F-format, significant digits for E-format
                             UTF16_DOT,                 // Char to use as decimal separator
                             UTF16_OVERBAR,             // Char to use as overbar
                             FLTDISPFMT_F);             // Float display format
            // Ensure properly terminated
            *--lpwEnd = WC_EOS;

            // Get ptr to decimal point
            lpwSrc = strchrW (lpwszFormat, UTF16_DOT);

            // If there's no decimal point, ...
            if (lpwSrc EQ NULL)
            {
                // Point to the end of the formatted number
                lpwSrc = lpwEnd;

                // Append a decimal point
                *lpwEnd++ = UTF16_DOT;

                // Ensure properly terminated
                *lpwEnd = WC_EOS;
            } // End IF

            // Skip over the decimal point
            lpwSrc++;

            Assert (fsDig >= (APLU3264) (lpwEnd - lpwSrc));

            // Reduce the desired # significant digits by the actual amount
            fsDig -= (lpwEnd - lpwSrc);

            // Fill the tail of the result with enough zeros
            //   to pad out to fsDig digits
            while (fsDig--)
                *lpwEnd++ = L'0';

            // Ensure properly terminated
            *lpwEnd = WC_EOS;

            break;

        case FMTSPECVAL_I:
            if (IsImmInt (immTypeItm))
                lpaplChar =
                  FormatAplintFC (lpwszFormat,          // Ptr to output save area
                                  aplIntItm,            // The value to format
                                  UTF16_OVERBAR);       // Char to use as overbar
            else
                lpaplChar =
                  FormatFloatFC (lpwszFormat,           // Ptr to output save area
                                 aplFltItm,             // The value to format
                                 fsWid,                 // Precision for F-format, significant digits for E-format
                                 UTF16_DOT,             // Char to use as decimal separator
                                 UTF16_OVERBAR,         // Char to use as overbar
                                 FLTDISPFMT_RAWINT);    // Float display format
            // Ensure properly terminated
            *--lpaplChar = WC_EOS;

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate length of and ptr to negative text to prepend (M-modifier)
    if (lpfsNxt->bM && aplFltItm <  0)
    {
        // Get ptr to M-modifier text
        lpSurrText->lpHead = (LPAPLCHAR) ByteAddr (lpfsNxt, lpfsNxt->offMtxt);

        // Get the M-modifier text length
        lpSurrText->uHead = lstrlenW (lpSurrText->lpHead);
    } // End IF

    // Calculate length of and ptr to negative text to append (N-modifier)
    if (lpfsNxt->bN && aplFltItm <  0)
    {
        // Get ptr to N-modifier text
        lpSurrText->lpTail = (LPAPLCHAR) ByteAddr (lpfsNxt, lpfsNxt->offNtxt);

        // Get the M-modifier text length
        lpSurrText->uTail = lstrlenW (lpSurrText->lpTail);
    } // End IF

    // Calculate length of and ptr to positive text to prepend (P-modifier)
    if (lpfsNxt->bP && aplFltItm >= 0)
    {
        // Get ptr to P-modifier text
        lpSurrText->lpHead = (LPAPLCHAR) ByteAddr (lpfsNxt, lpfsNxt->offPtxt);

        // Get the M-modifier text length
        lpSurrText->uHead = lstrlenW (lpSurrText->lpHead);
    } // End IF

    // Calculate length of and ptr to positive text to append (Q-modifier)
    if (lpfsNxt->bQ && aplFltItm >= 0)
    {
        // Get ptr to Q-modifier text
        lpSurrText->lpTail = (LPAPLCHAR) ByteAddr (lpfsNxt, lpfsNxt->offQtxt);

        // Get the M-modifier text length
        lpSurrText->uTail = lstrlenW (lpSurrText->lpTail);
    } // End IF

    // Insert thousands separator (C-modifier) (S-modifier)
    if (lpfsNxt->bC)
        QFMT_ThousandsSep (lpfsNxt, lpwszFormat, lpwSub, lpSurrText);
    else
    // Zero fill (Z-modifier) (S-modifier)
    if (lpfsNxt->bZ && !lpfsNxt->bL)
        QFMT_ZeroFill (lpfsNxt, lpwszFormat, lpwSub, lpSurrText);

    // Substitute symbols
    // At this point, no substitution has occurred so we can assume that
    //   the Decimal Point is UTF16_DOT
    //   the Thousands Separator is UTF16_COMMA
    //   the Exponent Separator is L'E', and
    //   the Zero Fill value is L'0'.

    // Do the substitution one char at a time so we don't confuse one
    //   already substituted symbol for another.
    lpwSymChar = lpwszFormat;
    bContinue = bZeroFill = TRUE;

    while (bContinue)
    switch (*lpwSymChar)
    {
        case UTF16_COMMA:
            *lpwSymChar++ = lpwSub[SYMSUB_THOUSANDS_SEP];

            break;

        case L'0':
            // The leading zero of a number whose absolute value is
            //   < 1 is not subject to Zero Fill
            if (bZeroFill && lpwSymChar[1] NE UTF16_DOT)
                *lpwSymChar++ = lpwSub[SYMSUB_ZERO_FILL];
            else
                lpwSymChar++;
            break;

        case L'1':
        case L'2':
        case L'3':
        case L'4':
        case L'5':
        case L'6':
        case L'7':
        case L'8':
        case L'9':
            lpwSymChar++;
            bZeroFill = FALSE;

            break;

        case UTF16_DOT:
            *lpwSymChar++ = lpwSub[SYMSUB_DECIMAL_SEP];
            bZeroFill = FALSE;

            break;

        case L'_':
            *lpwSymChar++ = lpwSub[SYMSUB_PRECISION_LOSS];
            bZeroFill = FALSE;

            break;

        case L'E':
            *lpwSymChar++ = lpwSub[SYMSUB_EXPONENT_SEP];
            bZeroFill = FALSE;

            break;

        case WC_EOS:
            bContinue = FALSE;

            break;

        default:
            lpwSymChar++;

            break;
    } // End SWITCH

    // Prepend with negative text (M-modifier)
    // Prepend with positive text (P-modifier)
    if ((lpfsNxt->bM && aplFltItm <  0)
     || (lpfsNxt->bP && aplFltItm >= 0))
    {
        UBOOL bNeg;                         // TRUE iff the formatted number is negative

        // Check for negatives
        bNeg = (lpwszFormat[0] EQ UTF16_OVERBAR);

        // Move the existing text up to make room for the M- or P-modifier text
        //   deleting the minus sign in the process (if M-modifier)
        // The "+ 1" is to copy the terminating zero
        MoveMemoryW (&lpwszFormat[lpSurrText->uHead],
                     &lpwszFormat[bNeg],
                      lstrlenW (&lpwszFormat[bNeg]) + 1);
        // Copy the M- or P-modifier text
        CopyMemoryW (lpwszFormat, lpSurrText->lpHead, lpSurrText->uHead);
    } // End IF

    // Append with negative text (N-modifier)
    // Append with positive text (Q-modifier)
    if ((lpfsNxt->bN && aplFltItm <  0)
     || (lpfsNxt->bQ && aplFltItm >= 0))
        lstrcatW (lpwszFormat, lpSurrText->lpTail);

    // Get the formatted item length
    iWid = lstrlenW (lpwszFormat);

    // If the item fits within the specified width, ...
    if (iWid <= fsWid)
    {
        // Left-justify (L-modifier)
        if (lpfsNxt->bL)
        {
            // Pad the end of the formatted item with blanks
            for (; iWid < fsWid; iWid++)
                lpwszFormat[iWid] = L' ';
            // Ensure properly terminated
            lpwszFormat[iWid] = WC_EOS;

            // iWid is the new formatted item length
        } // End IF

        // Copy text to the result
        CopyMemoryW (&lpMemRes[fsWid - iWid],
                      lpwszFormat,
                      iWid);
    } else
        // Insert overflow chars
        FillMemoryW (lpMemRes, fsWid, lpwSub[SYMSUB_OVERFLOW_FILL]);
} // End QFMT_CommonEFI


//***************************************************************************
//  $QFMT_ZeroFill
//
//  Fill the formatted number with leading zeros
//***************************************************************************

void QFMT_ZeroFill
    (LPFMTSPECSTR lpfsNxt,                  // Ptr to FMTSPECSTR
     LPAPLCHAR    lpwszFormat,              // Ptr to formatted item
     LPAPLCHAR    lpwSub,                   // Ptr to Symbol Substitution
     LPSURRTEXT   lpSurrText)               // Ptr to Surrounding Text info

{
    int     fsWid;                          // Field width
    UINT    uLen,                           // Length of the formatted number
            uHead;
    UBOOL   bNeg;                           // TRUE iff the formatted number has a negtive sign

    // Get the field width
    fsWid = lpfsNxt->fsWid - lpSurrText->uTail;

    // Check for negative
    bNeg = (lpwszFormat[0] EQ UTF16_OVERBAR);

    // Calculate the length of the leading text
    //   taking into account the negative sign
    if ((lpfsNxt->bP && !bNeg) || (lpfsNxt->bM && bNeg))
        uHead = lpSurrText->uHead;
    else
        uHead = bNeg;

    // Get the length of the formatted number
    //   less the negative sign (if any)
    //   to get the length to move leaving
    //   the negative sign in the first position
    uLen = lstrlenW (&lpwszFormat[bNeg]);

    // If it's too short, quit and let the caller take care of the problem
    if (fsWid < (int) (uLen + uHead))
        return;

    // Right-justify the item in the field width
    // The "+ 1" is to copy the terminating zero
    MoveMemoryW (&lpwszFormat[bNeg + fsWid - (uLen + uHead)],
                 &lpwszFormat[bNeg],
                  uLen + 1);
    // Fill with zeros
    FillMemoryW (&lpwszFormat[bNeg], fsWid - (uLen + uHead), L'0');
} // End QFMT_ZeroFill


//***************************************************************************
//  $QFMT_ThousandsSep
//
//  Insert thousands separators
//
//  Return the length before zero fill.
//***************************************************************************

void QFMT_ThousandsSep
    (LPFMTSPECSTR lpfsNxt,                  // Ptr to FMTSPECSTR
     LPAPLCHAR    lpwszFormat,              // Ptr to formatted item
     LPAPLCHAR    lpwSub,                   // Ptr to Symbol Substitution
     LPSURRTEXT   lpSurrText)               // Ptr to Surrounding Text info

{
    UINT    uSep1,                          // # separators
            uSep2,                          // ...
            uLen1,                          // Text length
            uLen2,                          // ...
            uDec;                           // Decimal point plus # digits to the right
    UBOOL   bNeg;                           // TRUE iff the formatted number has a negtive sign
    LPWCHAR lpwCur;                         // Ptr to current format element

    // Mark if negative sign
    bNeg = (lpwszFormat[0] EQ UTF16_OVERBAR);

    // Get the formatted item length
    uLen1 = uLen2 = lstrlenW (&lpwszFormat[bNeg]);

    // Count length of fractional part
    uDec = lstrlenW (SkipToCharW (&lpwszFormat[bNeg], UTF16_DOT));

    // Count # thousands separators to insert
    uSep1 = uSep2 = ((uLen1 - uDec) - 1) / 3;

    // Ensure properly terminated
    lpwszFormat[bNeg + uLen1 + uSep1] = WC_EOS;

    // Loop through the separators
    for (; uSep1; uLen1 -= 3, uSep1--)
    {
        // Calculate the destination ptr
        lpwCur = &lpwszFormat[bNeg + (uLen1 - uDec) - 3 + uSep1];

        // Move up the formatted integer to make room for one separator
        MoveMemoryW (lpwCur,
                    &lpwszFormat[bNeg + (uLen1 - uDec) - 3],
                     uDec + min (uLen1, 3));
        // Insert a thousands separator
        lpwCur[-1] = UTF16_COMMA;
    } // End FOR

    // If we're to fill with zeros, ...
    if (lpfsNxt->bZ)
    {
        int  fsWid;                         // Field width
        UINT uNegTxt;                       // Length of leading text less negative sign (if any)

        // Get the field width
        fsWid = lpfsNxt->fsWid - lpSurrText->uTail;

        // If it's too short, quit and let the caller take care of the problem
        if (fsWid <= 0)
            return;

        // Get leading text length less negative sign
        uNegTxt = lpSurrText->uHead - bNeg;

        // Calculate the destination ptr
        lpwCur = &lpwszFormat[fsWid - (uLen2 + uSep2 + uNegTxt)];

        // Right-justify the item in the field width
        // The "+ 1" is to copy the terminating zero
        MoveMemoryW (lpwCur,
                    &lpwszFormat[bNeg],
                     uLen2 + uSep2 + 1);
        // Specify how many digits within a group of three
        //   are already set in this group
        uLen2 = uLen2 % 3;

        // Handle initial condition
        if (uLen2 EQ 0
         && lpwCur > &lpwszFormat[bNeg])
            *--lpwCur = UTF16_COMMA;

        // While the field width still has room
        while (--lpwCur >= &lpwszFormat[bNeg])
        {
            if (uLen2 < 3)
            {
                *lpwCur = L'0';
                uLen2++;
            } else
            {
                *lpwCur = UTF16_COMMA;
                uLen2 = 0;
            } // End IF/ELSE
        } // End WHILE
    } // End IF
} // End QFMT_ThousandsSep


//***************************************************************************
//  $QFMT_FillBg
//
//  Fill []FMT result with background text
//***************************************************************************

void QFMT_FillBg
    (LPFMTSPECSTR lpfsNxt,                  // Ptr to FMTSPECSTR
     LPAPLCHAR    lpMemRes)                 // Ptr to result global memory

{
    UINT    uBkg,                           // Loop counter
            uWid,                           // FmtSpecStr width
            uLen;                           // Length of R-text
    LPWCHAR lpwSrc;                         // Ptr to R-text

    // Copy the width
    uWid = lpfsNxt->fsWid;

    // Get ptr to R-text and its length
    lpwSrc = (LPWCHAR) ByteAddr (lpfsNxt, lpfsNxt->offRtxt);
    uLen = lstrlenW (lpwSrc);
    uBkg = 0;

    if (uLen)
    while (uWid)
    {
        // Use the smaller length to avoid overwriting
        uLen = min (uLen, uWid);

        // Copy it to the output area
        CopyMemoryW (&lpMemRes[uBkg],
                      lpwSrc,
                      uLen);
        uWid -= uLen;
        uBkg += uLen;
    } // End WHILE
} // End QFMT_FillBg


//***************************************************************************
//  $QFMT_SubstValue
//
//  Substitute a value for []FMT arg O-modifier
//***************************************************************************

UBOOL QFMT_SubstValue
    (LPFMTSPECSTR lpfsNxt,                  // Ptr to FMTSPECSTR
     IMM_TYPES    immTypeItm,               // Item immediate type
     LPVOID       lpMemItm,                 // Ptr to item global memory
     APLINT       aplIntItm,                // Next value as an integer
     APLFLOAT     aplFltItm,                // ...              float
     LPAPLCHAR    lpMemRes)                 // Ptr to result global memory

{
    LPOCHAIN lpOchain;                      // Ptr to next O-chain
    APLFLOAT fQuadCT;                       // []CT

    // Search the O-chain for this integer
    lpOchain = (LPOCHAIN) ByteAddr (lpfsNxt, lpfsNxt->offOchain);

    // Split cases based upon the item immediate type
    switch (immTypeItm)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
            while (TRUE)
            if (lpOchain->aplIntVal EQ aplIntItm)
            {
                LPWCHAR lpwSub;         // Ptr to substitution text
                UINT    uWid,           // Field width
                        uLen;           // Text length

                // Get a ptr to the substitution text
                lpwSub = (LPWCHAR) ByteAddr (lpOchain, sizeof (OCHAIN));

                // Get the field width
                uWid = lpfsNxt->fsWid;

                // Get the text length
                uLen = lstrlenW (lpwSub);

                // Use the smaller of the text length and field width
                uLen = min (uWid, uLen);

                // If L-modifier is present, ...
                if (lpfsNxt->bL)
                    // Copy the substitution text, left -justified
                    CopyMemoryW ( lpMemRes,              lpwSub, uLen);
                else
                    // Copy the substitution text, right -justified
                    CopyMemoryW (&lpMemRes[uWid - uLen], lpwSub, uLen);

                return TRUE;
            } else
            if (lpOchain->offPrvChain)
                lpOchain = (LPOCHAIN) ByteAddr (lpfsNxt, lpOchain->offPrvChain);
            else
                return FALSE;

        case IMMTYPE_FLOAT:
            // Get the current value of []CT
            fQuadCT = GetQuadCT ();

            while (TRUE)
            if (CompareCT (lpOchain->aplFltVal, aplFltItm, fQuadCT, NULL))
            {
                LPWCHAR lpwSub;         // Ptr to substitution text
                UINT    uWid,           // Field width
                        uLen;           // Text length

                // Get a ptr to the substitution text
                lpwSub = (LPWCHAR) ByteAddr (lpOchain, sizeof (OCHAIN));

                // Get the field width
                uWid = lpfsNxt->fsWid;

                // Get the text length
                uLen = lstrlenW (lpwSub);

                // Use the smaller of the text length and field width
                uLen = min (uWid, uLen);

                // If L-modifier is present, ...
                if (lpfsNxt->bL)
                    // Copy the substitution text, left -justified
                    CopyMemoryW ( lpMemRes,              lpwSub, uLen);
                else
                    // Copy the substitution text, right -justified
                    CopyMemoryW (&lpMemRes[uWid - uLen], lpwSub, uLen);

                return TRUE;
            } else
            if (lpOchain->offPrvChain)
                lpOchain = (LPOCHAIN) ByteAddr (lpfsNxt, lpOchain->offPrvChain);
            else
                return FALSE;

        case IMMTYPE_CHAR:
        defstop
            return FALSE;
    } // End SWITCH
} // End QFMT_SubstValue


//***************************************************************************
//  End of File: qf_fmt.c
//***************************************************************************
