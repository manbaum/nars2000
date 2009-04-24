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
  L"Z" $IS MFN_MonFMT L" R;" $QUAD L"io;Type;Lft;Top;Proto;A;D";

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
  L"  Type" $IS L"(0 ' '" $EPSILON $QUAD L"type Proto)/'~" $LDC_LT_HORZ $NEG L"'[0,1+0" $EPSILON $RHO L"R]";

static APLCHAR MonLineFMT11[] =
  L"  :select ×/" $RHO L"Type";

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
  L"    Z" $IS L"Z,' " $ZILDE L"'[Type='~']";

static APLCHAR MonLineFMT32[] =
  L"  :endif";

static APLCHAR MonLineFMT33[] =
  L"  :while 2<" $RHO $RHO L"Z";

static APLCHAR MonLineFMT34[] =
  L"    Z" $IS $NEG L"1" $DROP L"[" $NEG L"2],[0 1] Z,[" $NEG L"2]' '";

static APLCHAR MonLineFMT35[] =
  L"  :endwhile";

static APLCHAR MonLineFMT36[] =
  L"  Z" $IS L"Z #Box R";

static APLCHAR MonLineFMT37[] =
  L":else";

static APLCHAR MonLineFMT38[] =
  L"  :if 0" $EPSILON $RHO L"R";

static APLCHAR MonLineFMT39[] =
  L"    Z" $IS L"#MonFMT " $TAKE L"R";

static APLCHAR MonLineFMT40[] =
  L"    Z" $IS L"Z #Box R";

static APLCHAR MonLineFMT41[] =
  L"  :else";

static APLCHAR MonLineFMT42[] =
  L"    Z" $IS L"''";

static APLCHAR MonLineFMT43[] =
  L"    :forlcl I :in R";

static APLCHAR MonLineFMT44[] =
  L"      A" $IS L"#MonFMT I";

static APLCHAR MonLineFMT45[] =
  L"      D" $IS $MATCH L"I";

static APLCHAR MonLineFMT46[] =
  L"      :if 0=D";

static APLCHAR MonLineFMT47[] =
  L"        A" $IS L"' '" $COMMABAR L"A";

static APLCHAR MonLineFMT48[] =
  L"      :endif";

static APLCHAR MonLineFMT49[] =
  L"      Z" $IS L"Z," $ENCLOSE L"A";

static APLCHAR MonLineFMT50[] =
  L"    :endforlcl";

static APLCHAR MonLineFMT51[] =
  L"    Z" $IS L"(' '," $DISCLOSE L",/(0" $DISCLOSE $DISCLOSE $MAX L"/" $RHO L"¨Z)"
    $TAKE L"[0]¨Z,¨' ') #Box R";

static APLCHAR MonLineFMT52[] =
  L"  :endif";

static APLCHAR MonLineFMT53[] =
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
  L"Z" $IS L"(" $NEG L"2" $TAKE L"1," $RHO L"Z)" $RHO L"Z " $DIAMOND L" Z" $IS L"((" $RHO L"Z)"
       $MAX L"(" $NEG L"1" $TAKE $RHO L"RD)," $RHO L"LD)" $TAKE L"Z";

static APLCHAR MonLineBox5[] =
  L":if 1" $GE $RHO $RHO L"R";

static APLCHAR MonLineBox6[] =
  L"  RD" $IS L"((1" $TAKE $RHO L"Z),1)" $RHO L"'" $LDC_LT_VERT L"'";

static APLCHAR MonLineBox7[] =
  L":else";

static APLCHAR MonLineBox8[] =
  L"  RD" $IS $TRANSPOSE L"(1" $TAKE $RHO L"Z)" $TAKE L"[1] RD "
  $DIAMOND L" RD[(,RD=' ')/," $IOTA $RHO L"RD]" $IS L"'" $LDC_LT_VERT L"'";

static APLCHAR MonLineBox9[] =
  L":endif";

static APLCHAR MonLineBox10[] =
  L"RD" $IS L"'" $LDC_LT_HORZ_D L"'" $COMMABAR L"RD" $COMMABAR L"'" $LDC_LT_HORZ_U L"' "
  $DIAMOND L" RD[0 " $NEG L"1;0]" $IS L"'" $LDC_LT_UL $LDC_LT_LL L"'";

static APLCHAR MonLineBox11[] =
  L"Z" $IS L"Z,'" $LDC_LT_VERT L"'";

static APLCHAR MonLineBox12[] =
  L"D" $IS $MATCH L"R";

static APLCHAR MonLineBox13[] =
  L":if 1<D";

static APLCHAR MonLineBox14[] =
  L"  D" $IS $FORMAT L"D";

static APLCHAR MonLineBox15[] =
  L"  Z[" $IOTA L"-" $RHO L"D;" $NEG L"1]" $IS $ROTATE L"D";

static APLCHAR MonLineBox16[] =
  L":endif";

static APLCHAR MonLineBox17[] =
  L"Z" $IS L"'" $LDC_LT_HORZ L"'" $COMMABAR L"Z" $COMMABAR L"'" $LDC_LT_HORZ L"'";

static APLCHAR MonLineBox18[] =
  L"Z[0 " $NEG L"1;" $NEG L"1]" $IS L"'" $LDC_LT_UR $LDC_LT_LR L"'";

static APLCHAR MonLineBox19[] =
  L"Z[0;" $IOTA $RHO L"LD]" $IS L"LD";

static APLCHAR MonLineBox20[] =
  L"Z[" $NEG L"1;0]" $IS L"Type";

static APLCHAR MonLineBox21[] =
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
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnNonceError_EM (lptkFunc);
} // End SysFnDydFMT_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_fmt.c
//***************************************************************************
