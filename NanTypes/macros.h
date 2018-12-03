//****************************************************************************
//  NARS2000 -- Macros File
//****************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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


#define MB(a)                       MessageBox  (NULL, (a),      APPNAME, MB_OK)
#define MBW(a)                      MessageBoxW (hWndMF, (a), WS_APPNAME, MB_OK)
#define MBPW(a,b)                   MessageBoxW (hWndMF, (a), WS_APPNAME, MB_OK | (b))  // With parameter
#define MBC(a)                  {if (MessageBox  (NULL, (a),      APPNAME, MB_OKCANCEL) EQ IDCANCEL) DbgBrk ();}
#define MBWC(a)                 {if (MessageBoxW (hWndMF, (a), WS_APPNAME, MB_OKCANCEL) EQ IDCANCEL) DbgBrk ();}
#define IsGlbTypeVarDir_PTB(a)  (IsGlobalTypeArray_PTB (            (a), VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeVarInd_PTB(a)  (IsGlobalTypeArray_PTB (*(LPVOID *) (a), VARARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnDir_PTB(a)  (IsGlobalTypeArray_PTB (            (a), FCNARRAY_HEADER_SIGNATURE))
#define IsGlbTypeFcnInd_PTB(a)  (IsGlobalTypeArray_PTB (*(LPVOID *) (a), FCNARRAY_HEADER_SIGNATURE))
#define IsGlbTypeDfnDir_PTB(a)  (IsGlobalTypeArray_PTB (            (a), DFN_HEADER_SIGNATURE))
#define IsGlbTypeNamDir_PTB(a)  (IsGlobalTypeArray_PTB (            (a), VARNAMED_HEADER_SIGNATURE))
#define IsGlbTypeLstDir_PTB(a)  (IsGlobalTypeArray_PTB (            (a), LSTARRAY_HEADER_SIGNATURE))
#define IsSymNoValue(a)         ((a) EQ NULL || ((a)->stFlags.Value EQ FALSE && (a)->stFlags.FcnDir EQ FALSE))
#define IsTokenNoValue(a)       (((a) NE NULL)                          \
                              && ((a)->tkFlags.TknType EQ TKT_VARNAMED) \
                              && (IsSymNoValue ((a)->tkData.tkSym)))
#define IsMFOName(a)            ((a)[0] EQ L'#')
#define IsSysName(a)            ((a)[0] EQ UTF16_QUAD  || (a)[0] EQ UTF16_QUAD2 || (a)[0] EQ UTF16_QUOTEQUAD)
#define IsDirectName(a)         ((a)    EQ UTF16_ALPHA || (a)    EQ UTF16_OMEGA || (a)    EQ UTF16_DEL)

#define ByteAddr(a,b)           (&(((LPBYTE) (a))[b]))
#define ByteDiff(a,b)           (((LPBYTE) (a)) - (LPBYTE) (b))

#define LOLONGLONG(x)           ( (LONG) ( ( ( (LONGLONG) x) & LOPART_LONGLONG)      ) )
#define HILONGLONG(x)           ( (LONG) ( ( ( (LONGLONG) x) & HIPART_LONGLONG) >> 32) )

#define LODWORD(x)              ( (DWORD) (   (x) & LOPART_DWORDLONG ) )
#define HIDWORD(x)              ( (DWORD) ( ( (x) & HIPART_DWORDLONG ) >> 32 ) )

#define LOSHORT(l)              ((short)((DWORD)(l) & 0xffff))
#define HISHORT(l)              ((short)((DWORD)(l) >> 16))

#define LOAPLUINT(a)            (((LPAPLU_SPLIT) &a)->lo)
#define HIAPLUINT(a)            (((LPAPLU_SPLIT) &a)->hi)

#define LOAPLINT(a)             (((LPAPLI_SPLIT) &a)->lo)
#define HIAPLINT(a)             (((LPAPLI_SPLIT) &a)->hi)

#define CheckSymEntries()       _CheckSymEntries (FNLN)

#define DebugFile(a,b)          _DebugFile (a, b, FNLN)

#ifdef DEBUG
//#define DEBUG_ALLOCFREE

  #define YYAlloc()     _YYAlloc (FNLN)
  #define YYFree(a)     _YYFree (a, FNLN)

  #ifdef DEBUG_ALLOCFREE
    #define DbgGlobalAlloc(uFlags,ByteRes) \
    DbgGlobalAllocSub ((uFlags), (ByteRes), L"##GlobalAlloc in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgGlobalFree(hGlbToken) \
    DbgGlobalFreeSub ((hGlbToken), L"**GlobalFree  in " APPEND_NAME L": %p (%S#%d)", FNLN)
  #else
    #define DbgGlobalAlloc(uFlags,ByteRes) \
    MyGlobalAlloc ((uFlags), (ByteRes))

    #define DbgGlobalFree(hGlbToken) \
    MyGlobalFree (hGlbToken)
  #endif

  #ifdef DEBUG_REFCNT
    #define DbgIncrRefCntDir_PTB(hGlbData) \
    _DbgIncrRefCntDir_PTB (hGlbData, L"##RefCnt++ in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgIncrRefCntInd_PTB(hGlbData) \
    _DbgIncrRefCntInd_PTB (hGlbData, L"##RefCnt++ in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgIncrRefCntTkn(lptkVar)      \
    _DbgIncrRefCntTkn     (lptkVar,  L"##RefCnt++ in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgChangeRefCntTkn(lptkVar,iChangeRefCnt) \
    _DbgChangeRefCntTkn   (lptkVar,  iChangeRefCnt, L"##RefCnt%d in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgIncrRefCntFcnArray(hGlbFcn) \
    _DbgIncrRefCntFcnArray(hGlbFcn,  L"##RefCnt++ in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgDecrRefCntFcnArray(hGlbFcn) \
    _DbgDecrRefCntFcnArray(hGlbFcn,  L"##RefCnt-- in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgDecrRefCntDir_PTB(hGlbData) \
    _DbgDecrRefCntDir_PTB (hGlbData, L"##RefCnt-- in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgDecrRefCntInd_PTB(hGlbData) \
    _DbgDecrRefCntInd_PTB (hGlbData, L"##RefCnt-- in " APPEND_NAME L": %p (%S#%d)", FNLN)

    #define DbgDecrRefCntTkn(lptkVar)      \
    _DbgDecrRefCntTkn     (lptkVar,  L"##RefCnt-- in " APPEND_NAME L": %p (%S#%d)", FNLN)
  #else
    #define DbgIncrRefCntDir_PTB(hGlbData) \
    IncrRefCntDir_PTB (hGlbData)

    #define DbgIncrRefCntInd_PTB(hGlbData) \
    IncrRefCntInd_PTB (hGlbData)

    #define DbgIncrRefCntTkn(lptkVar) \
    IncrRefCntTkn (lptkVar)

    #define DbgChangeRefCntTkn(lptkVar,iChangeRefCnt) \
    ChangeRefCntTkn (lptkVar, iChangeRefCnt)

    #define DbgIncrRefCntFcnArray(hGlbFcn) \
    IncrRefCntFcnArray (hGlbFcn)

    #define DbgDecrRefCntFcnArray(hGlbFcn) \
    DecrRefCntFcnArray (hGlbFcn)

    #define DbgDecrRefCntDir_PTB(hGlbData) \
    DecrRefCntDir_PTB (hGlbData)

    #define DbgDecrRefCntInd_PTB(hGlbData) \
    DecrRefCntInd_PTB (hGlbData)

    #define DbgDecrRefCntTkn(lptkVar) \
    DecrRefCntTkn (lptkVar)
  #endif

  #define DbgMsgW2(a)                     {if (gDbgLvl > 2) {DbgMsgW(a);}}

  #define Assert(a)                       ((a) || (DbgBrk (), nop (), 0))
  #define ExecAssert(a)                   Assert (a)
  #define BreakIf(a)                      Assert (!(a))
//#define nop()                           // Use already defined function
//#define DbgNop()                        // Use already defined function
  #define YYCheckInuse(a)                 _YYCheckInuse (a, FNLN)
  #define CheckCtrlBreak(a)               _CheckCtrlBreak (a)
//#define DisplayGlbVar(a,b)              DisplayGlbVar (a, b)

  #define MySetFocus(a)                   _MySetFocus((a), FNLN)
#else
  #define YYAlloc()     _YYAlloc ()
  #define YYFree(a)     _YYFree (a)

  #define DbgGlobalAlloc(uFlags,ByteRes)  MyGlobalAlloc ((uFlags), (ByteRes))

  #define DbgGlobalFree(hGlbToken)        MyGlobalFree (hGlbToken)

  #define DbgIncrRefCntDir_PTB(hGlbData)  IncrRefCntDir_PTB (hGlbData)

  #define DbgIncrRefCntInd(hGlbData)      IncrRefCntInd (hGlbData)

  #define DbgIncrRefCntTkn(lptkVar)       IncrRefCntTkn (lptkVar)

  #define DbgChangeRefCntTkn(lptkVar,iChangeRefCnt)  ChangeRefCntTkn (lptkVar, iChangeRefCnt)

  #define DbgIncrRefCntFcnArray(hGlbFcn)  IncrRefCntFcnArray (hGlbFcn)

  #define DbgDecrRefCntFcnArray(hGlbFcn)  DecrRefCntFcnArray (hGlbFcn)

  #define DbgDecrRefCntDir_PTB(hGlbData)  DecrRefCntDir_PTB (hGlbData)

  #define DbgDecrRefCntInd(hGlbData)      DecrRefCntInd (hGlbData)

  #define DbgDecrRefCntTkn(lptkVar)       DecrRefCntTkn (lptkVar)

  #define DbgMsg(a)
  #define DbgMsgW(a)
  #define DbgMsgW2(a)

  #define Assert(a)                       ((void) 0)
  #define ExecAssert(a)                   (a)
  #define BreakIf(a)                      ((void) 0)
////  #define Assert(a) ((a) || (AssertPrint(#a, FNLN), 0))
  #define nop()                           {}    // Nothing
  #define DbgNop()                        {}    // Nothing
  #define YYCheckInuse(a)                 {}    // Nothing
  #define CheckCtrlBreak(a)               (*a)
  #define DisplayGlbVar(a,b)

  #define dprintfWL0(a,...)
  #define dprintfWL9(a,...)

  #define MySetFocus(a)                   SetFocus(a)
#endif

#if RESDEBUG
  #define CheckMemStat()                  _CheckMemStat ()
  #define CheckGlbStat(a)                 _CheckGlbStat (a)
#else
  #define CheckMemStat()
  #define CheckGlbStat(a)
#endif

#define imul64_RE(a,b,c)    imul64 ((a), (b), NULL, EXCEPTION_RESULT_##c)
#define iadd64_RE(a,b,c)    iadd64 ((a), (b), NULL, EXCEPTION_RESULT_##c)
#define isub64_RE(a,b,c)    isub64 ((a), (b), NULL, EXCEPTION_RESULT_##c)

// []FEATURE values
#define gAllowNegIndices    GetMemPTD()->aplCurrentFEATURE[FEATURENDX_NEGINDICES ]
#define gAllowNeg0          GetMemPTD()->aplCurrentFEATURE[FEATURENDX_NEG0       ]
#define gUseHurwitz         GetMemPTD()->aplCurrentFEATURE[FEATURENDX_HURWITZ    ]
#define gAllowNaN           GetMemPTD()->aplCurrentFEATURE[FEATURENDX_NAN        ]
#define gUseAPL2IP          GetMemPTD()->aplCurrentFEATURE[FEATURENDX_IP2        ]
#define gbAllowNegIndices    lpMemPTD  ->aplCurrentFEATURE[FEATURENDX_NEGINDICES ]  // Use these if lpMemPTD is already assigned
#define gbAllowNeg0          lpMemPTD  ->aplCurrentFEATURE[FEATURENDX_NEG0       ]  // ...
#define gbUseHurwitz         lpMemPTD  ->aplCurrentFEATURE[FEATURENDX_HURWITZ    ]  // ...
#define gbAllowNaN           lpMemPTD  ->aplCurrentFEATURE[FEATURENDX_NAN        ]  // ...
#define gbUseAPL2IP          lpMemPTD  ->aplCurrentFEATURE[FEATURENDX_IP2        ]  // ...

////ine signumint(a)        (   (((APLINT) (a)) < 0) ? -1 : (((APLINT) (a)) > 0))
////ine signumflt(a)        (SIGN_APLFLOAT (a)       ? -1 : (          (a)  > 0))
#define SIGN_APLNELM(a)     ((UBOOL) ((a) >> 63))                   // Sign bit of an APLNELM
#define SIGN_APLRANK(a)     ((UBOOL) ((a) >> 63))                   // ...            APLRANK
#define SIGN_APLDIM(a)      ((UBOOL) ((a) >> 63))                   // ...            APLDIM
#define SIGN_APLINT(a)      ((UBOOL) (((APLUINT) (a)) >> 63))       // ...            APLINT
#define SIGN_APLUINT(a)     ((UBOOL) ((a) >> 63))                   // ...            APLUINT
#define SIGN_APLLONGEST(a)  ((UBOOL) ((a) >> 63))                   // ...            APLLONGEST
#define SIGN_APLFLOAT_RAW(a) ((UBOOL) (((LPAPLFLOATSTR) &a)->bSign))// ...            APLFLOAT
#define SIGN_APLFLOAT(a)    (gAllowNeg0 ? SIGN_APLFLOAT_RAW (a)                     \
                                        : ((a) < 0))                // ...            APLFLOAT
#define SIGN_APLRAT(a)      (mpq_sgn (a) < 0)                       // ...            APLRAT
#define SIGN_APLVFP_RAW(a)  ((a)->_mpfr_sign < 0)                   // ...            APLVFP
#define SIGN_APLVFP(a)      (gAllowNeg0 ? SIGN_APLVFP_RAW (a)                       \
                                        : (mpfr_sgn (a) < 0))       // ...            APLVFP
#define SIGN_APLARB_RAW(a)  ((a)->mid.size < 0)                     // ...            APLVFP
#define SIGN_APLARB(a)      (FALSE      ? SIGN_APLARB_RAW (a)                       \
                                        : (arb_sign (a) < 0))       // ...            APLVFP
#define signumrat            mpq_sgn
#define signumvfp(a)        (SIGN_APLVFP   (a)       ? -1 : ( mpfr_sgn (a)  > 0))
#define mpfr_sgn0(a)        (signumint ((a)->_mpfr_sign))
#define signumarf            arf_sgn
#define SIGN_APLARF(a)      (signumarf (a) < 0)

// Define macro to detect -0.0
#define IsFltN0(a)          ((a) EQ 0.0  && SIGN_APLFLOAT ( a))

// Define macro to detect -0.0v
#define IsVfpN0(a)          (IsMpf0 (&a) && SIGN_APLVFP   (&a))

// Define macros for detecting characters and their alternates
#define IsAPLCharAlpha(a)           ((a) EQ UTF16_ALPHA              || (a) EQ UTF16_ALPHA2                                        )
#define IsAPLCharOmega(a)           ((a) EQ UTF16_OMEGA              || (a) EQ UTF16_OMEGA2                                        )
#define IsAPLCharBar(a)             ((a) EQ UTF16_BAR                || (a) EQ UTF16_BAR2                                          )
#define IsAPLCharStile(a)           ((a) EQ UTF16_STILE              || (a) EQ UTF16_STILE2                                        )
#define IsAPLCharEpsilon(a)         ((a) EQ UTF16_EPSILON            || (a) EQ UTF16_EPSILON2                                      )
#define IsAPLCharTilde(a)           ((a) EQ UTF16_TILDE              || (a) EQ UTF16_TILDE2                                        )
#define IsAPLCharCircle(a)          ((a) EQ UTF16_CIRCLE             || (a) EQ UTF16_CIRCLE2                                       )
#define IsAPLCharDiamond(a)         ((a) EQ UTF16_DIAMOND            || (a) EQ UTF16_DIAMOND2             || (a) EQ UTF16_DIAMOND3 || (a) EQ UTF16_DIAMOND4)
#define IsAPLCharStar(a)            ((a) EQ UTF16_STAR               || (a) EQ UTF16_STAR2                                         )
#define IsAPLCharJot(a)             ((a) EQ UTF16_JOT                || (a) EQ UTF16_JOT2                                          )
#define IsAPLCharQuad(a)            ((a) EQ UTF16_QUAD               || (a) EQ UTF16_QUAD2                                         )
#define IsAPLCharNotMore(a)         ((a) EQ UTF16_LEFTCARETUNDERBAR  || (a) EQ UTF16_LEFTCARETUNDERBAR2                            )
#define IsAPLCharNotLess(a)         ((a) EQ UTF16_RIGHTCARETUNDERBAR || (a) EQ UTF16_RIGHTCARETUNDERBAR2                           )
#define IsAPLCharUpCaretTilde(a)    ((a) EQ UTF16_UPCARETTILDE       || (a) EQ UTF16_NAND                                          )
#define IsAPLCharDownCaretTilde(a)  ((a) EQ UTF16_DOWNCARETTILDE     || (a) EQ UTF16_NOR                                           )
#define IsAPLCharUpCaret(a)         ((a) EQ UTF16_UPCARET            || (a) EQ UTF16_CIRCUMFLEX                                    )

// Define macro for detecting Nonce array type
#define IsNonceType(ArrType)            (ArrType EQ ARRAY_NONCE)

// Define macro for detecting Real  array type
#define IsRealType(ArrType)             (ArrType EQ ARRAY_REALONLY)

// Define macro for detecting Rad0  array type
#define IsRad0Type(ArrType)             (ARRAY_RAD0RONLY <= ArrType && ArrType <= ARRAY_RAD0OONLY)

// Define macro for detecting Error array type
#define IsErrorType(ArrType)            (IsNonceType (ArrType) || IsRealType (ArrType) || IsRad0Type (ArrType) || ArrType EQ ARRAY_ERROR)

// Define macro for detecting Simple array type
/////// IsSimple(ArrType)               ((ArrType) EQ ARRAY_BOOL || (ArrType) EQ ARRAY_INT || (ArrType) EQ ARRAY_APA || (ArrType) EQ ARRAY_FLOAT || (ArrType) EQ ARRAY_CHAR || (ArrType) EQ ARRAY_HETERO)
#define IsSimple(ArrType)               ((!IsErrorType (ArrType)) && uTypeMap[ArrType] <= uTypeMap[ARRAY_HETERO])

// Define macro for detecting Simple non-heterogeneous array type
/////// IsSimpleNH(ArrType)             ((ArrType) EQ ARRAY_BOOL || (ArrType) EQ ARRAY_INT || (ArrType) EQ ARRAY_APA || (ArrType) EQ ARRAY_FLOAT || (ArrType) EQ ARRAY_CHAR)
#define IsSimpleNH(ArrType)             ((!IsErrorType (ArrType)) && uTypeMap[ArrType] < uTypeMap[ARRAY_HETERO])

// Define macro for detecting Simple numeric array type
/////// IsSimpleNum(ArrType)            ((ArrType) EQ ARRAY_BOOL || (ArrType) EQ ARRAY_INT || (ArrType) EQ ARRAY_APA || (ArrType) EQ ARRAY_FLOAT)
#define IsSimpleNum(ArrType)            ((!IsErrorType (ArrType)) && uTypeMap[ArrType] < uTypeMap[ARRAY_CHAR])

// Define macro for detecting Simple Boolean array type
#define IsSimpleBool(ArrType)           ((ArrType) EQ ARRAY_BOOL)

// Define macro for detecting Simple integer-like array type
#define IsRealBIA(ArrType)              ((ArrType) EQ ARRAY_BOOL || (ArrType) EQ ARRAY_INT || (ArrType) EQ ARRAY_APA)

// Define macro for detecting Simple APA array type
#define IsSimpleAPA(ArrType)            ((ArrType) EQ ARRAY_APA)

// Define macro for detecting Boolean APAs
#define IsBooleanAPA(lpAPA)             ((lpAPA) && ((lpAPA)->Mul EQ 0) && IsBooleanValue ((lpAPA)->Off))

// Define macro for detecting Simple float array type
#define IsRealFlt(ArrType)              ((ArrType) EQ ARRAY_FLOAT)

// Define macro for detecting Numeric arrays (including RAT, VFP, ARB, etc.)
#define IsNumeric(ArrType)              (IsSimpleNum (ArrType) || IsGlbNum (ArrType))

// Define macro for detecting Character or Numeric arrays (including RAT, VFP, ARB, etc.)
#define IsSimpleGlbNum(ArrType)         (IsSimple (ArrType) || IsGlbNum (ArrType))

// Define macro for detecting Character or Numeric arrays (including RAT, VFP, ARB, etc.) but not hetero
#define IsSimpleNHGlbNum(ArrType)       (IsSimpleNH (ArrType) || IsGlbNum (ArrType))

// Define macro for detecting Global Numeric arrays (including RAT, VFP, ARB, etc.)
#define IsGlbNum(ArrType)               (ARRAY_RAT <= (ArrType) && (ArrType) <= ARRAY_BA8F)

// Define macro for detecting Simple character array type
#define IsSimpleChar(ArrType)           ((ArrType) EQ ARRAY_CHAR)

// Define macro for detecting Simple character or heterogeneous array type
#define IsSimpleCH(ArrType)             ((ArrType) EQ ARRAY_CHAR || (ArrType) EQ ARRAY_HETERO)

// Define macro for detecting a simple character AND empty array
#define IsCharEmpty(ArrType,ArrNELM)    (IsSimpleChar (ArrType) && IsEmpty (ArrNELM))

// Define macro for detecting a simple character OR  empty array
#define IsCharOrEmpty(ArrType,ArrNELM)  (IsSimpleChar (ArrType) || IsEmpty (ArrNELM))

// Define macro for detecting simple hetero array type
#define IsSimpleHet(ArrType)            ((ArrType) EQ ARRAY_HETERO)

// Define macro for detecting nested array type
#define IsNested(ArrType)               ((ArrType) EQ ARRAY_NESTED)

// Define macro for detecting nested or hetero (that is, ptr) arrays
#define IsPtrArray(ArrType)             (IsSimpleHet (ArrType) || IsNested (ArrType))

// Define macro for detecting list array type
#define IsList(ArrType)                 ((ArrType) EQ ARRAY_LIST)

// Define macro for detecting a Real-only Rat
#define IsRealRat(ArrType)              ((ArrType) EQ ARRAY_RAT)

// Define macro for detecting a Real-only Variable FP
#define IsRealVfp(ArrType)              ((ArrType) EQ ARRAY_VFP)

// Define macro for detecting a Real-only Arb
#define IsRealArb(ArrType)              ((ArrType) EQ ARRAY_ARB)

// Define macro for detecting any Hypercomplex-only HCxI
#define IsHCxI(ArrType)                 ((ArrType) EQ ARRAY_HC2I || (ArrType) EQ ARRAY_HC4I || (ArrType) EQ ARRAY_HC8I)

// Define macro for detecting any INT including Hypercomplex
#define IsAnyInt(ArrType)               (IsRealBIA (ArrType)     || IsHCxI (ArrType))

// Define macro for detecting any Hypercomplex-only HCxF
#define IsHCxF(ArrType)                 ((ArrType) EQ ARRAY_HC2F || (ArrType) EQ ARRAY_HC4F || (ArrType) EQ ARRAY_HC8F)

// Define macro for detecting any FLT including Hypercomplex
#define IsAnyFlt(ArrType)               (IsRealFlt (ArrType)     || IsHCxF (ArrType))

// Define macro for detecting any Hypercomplex-only HCxR
#define IsHCxR(ArrType)                 ((ArrType) EQ ARRAY_HC2R || (ArrType) EQ ARRAY_HC4R || (ArrType) EQ ARRAY_HC8R)

// Define macro for detecting any Rat including Hypercomplex
#define IsAnyRat(ArrType)               (IsRealRat (ArrType)     || IsHCxR (ArrType))

// Define macro for detecting any Hypercomplex-only HCxV
#define IsHCxV(ArrType)                 ((ArrType) EQ ARRAY_HC2V || (ArrType) EQ ARRAY_HC4V || (ArrType) EQ ARRAY_HC8V)

// Define macro for detecting any Variable FP including Hypercomplex
#define IsAnyVfp(ArrType)               (IsRealVfp (ArrType)     || IsHCxV (ArrType))

// Define macro for detecting any Hypercomplex-only Arb
#define IsHCArb(ArrType)                ((ArrType) EQ ARRAY_BA2F || (ArrType) EQ ARRAY_BA4F || (ArrType) EQ ARRAY_BA8F)

// Define macro for detecting any Arb including Hypercomplex
#define IsAnyArb(ArrType)               (IsRealArb (ArrType)     || IsHCArb (ArrType))

// Define macro for detecting any Hypercomplex-only number
#define IsHCAny(ArrType)                (IsHCxI (ArrType) || IsHCxF (ArrType) || IsHCxR (ArrType) || IsHCxV (ArrType) || IsHCArb (ArrType))

// Define macro for detecting any numeric including Hypercomplex
#define IsAnyNum(ArrType)               (IsSimpleNum (ArrType) || IsHCAny (ArrType))

// Define macro for detecting an BA1F Midpoint and radius of 0
#define IsArb0(Arb)                     (arb_is_zero (Arb) NE 0)

// Define macro for detecting an BA1F Midpoint of 1 and radius of 0
#define IsArb1(Arb)                     (arb_is_one  (Arb) NE 0)

// Define macro for detecting an BA1F Midpoint of 0, 1, NaN, or �Infinity
#define IsArf0(Arf)                     (arf_is_zero (Arf) NE 0)
#define IsArf1(Arf)                     (arf_is_one  (Arf) NE 0)
#define IsArfNaN(Arf)                   (arf_is_nan  (Arf) NE 0)
#define IsArfInf(Arf)                   (arf_is_inf  (Arf) NE 0)
#define IsArbPosInfinity(a)             (arf_is_pos_inf (arb_midref (a)))
#define IsArbInfinity(a)                (arf_is_inf     (arb_midref (a)))
#define IsArbNegInfinity(a)             (arf_is_neg_inf (arb_midref (a)))

#ifndef DEBUG
#define SqNrmHC2I(a)                    SqNrmHCxI ((a), 2)
#define SqNrmHC4I(a)                    SqNrmHCxI ((a), 4)
#define SqNrmHC8I(a)                    SqNrmHCxI ((a), 8)

#define SqNrmHC2F(a)                    SqNrmHCxF ((a), 2)
#define SqNrmHC4F(a)                    SqNrmHCxF ((a), 4)
#define SqNrmHC8F(a)                    SqNrmHCxF ((a), 8)

#define SqNrmHC2R(a)                    SqNrmHCxR ((a), 2)
#define SqNrmHC4R(a)                    SqNrmHCxR ((a), 4)
#define SqNrmHC8R(a)                    SqNrmHCxR ((a), 8)

#define SqNrmHC2V(a)                    SqNrmHCxV ((a), 2)
#define SqNrmHC4V(a)                    SqNrmHCxV ((a), 4)
#define SqNrmHC8V(a)                    SqNrmHCxV ((a), 8)

#define SqNrmBA2F(a)                    SqNrmBAxF ((a), 2)
#define SqNrmBA4F(a)                    SqNrmBAxF ((a), 4)
#define SqNrmBA8F(a)                    SqNrmBAxF ((a), 8)
#endif

#ifdef DEBUG
// Define macros for arbX_init
#define Myarb_init(a)                   arb1_init (a)
#define arb1f_init(a)                   arb1_init (a)
#define arb2f_init(a)                   arb2_init (a)
#define arb4f_init(a)                   arb4_init (a)
#define arb8f_init(a)                   arb8_init (a)
#else
// Define macros for arbX_init
#define Myarb_init(a)                   arbX_init ((a), 1)
#define arb1f_init(a)                   arbX_init ((a), 1)
#define arb2f_init(a)                   arbX_init ((a), 2)
#define arb4f_init(a)                   arbX_init ((a), 4)
#define arb8f_init(a)                   arbX_init ((a), 8)
#endif

#ifdef DEBUG
// Define macros for arbX_init_set
#define arb1f_init_set                  arb_init_set
#define arb2f_init_set(a,b)             arb2_init_set ((a), (b))
#define arb4f_init_set(a,b)             arb4_init_set ((a), (b))
#define arb8f_init_set(a,b)             arb8_init_set ((a), (b))
#else
// Define macros for arbX_init_set
#define arb1f_init_set                  arb_init_set
#define arb2f_init_set(a,b)             arbX_init_set ((a), (b), 2)
#define arb4f_init_set(a,b)             arbX_init_set ((a), (b), 4)
#define arb8f_init_set(a,b)             arbX_init_set ((a), (b), 8)
#endif

#ifdef DEBUG
// Define macros for arbX_init_set_sx
#define arb1f_init_set_sx               arb_init_set_sx
#define arb2f_init_set_sx(a,b)          arb2_init_set_sx ((a), (b))
#define arb4f_init_set_sx(a,b)          arb4_init_set_sx ((a), (b))
#define arb8f_init_set_sx(a,b)          arb8_init_set_sx ((a), (b))
#else
// Define macros for arbX_init_set_sx
#define arb1f_init_set_sx               arb_init_set_sx
#define arb2f_init_set_sx(a,b)          arbX_init_set_sx ((a), (b), 2)
#define arb4f_init_set_sx(a,b)          arbX_init_set_sx ((a), (b), 4)
#define arb8f_init_set_sx(a,b)          arbX_init_set_sx ((a), (b), 8)
#endif

#ifdef DEBUG
// Define macros for arbX_clear
#define Myarb_clear(a)                  arb1_clear (a)
#define arb1f_clear(a)                  arb1_clear (a)
#define arb2f_clear(a)                  arb2_clear (a)
#define arb4f_clear(a)                  arb4_clear (a)
#define arb8f_clear(a)                  arb8_clear (a)
#else
// Define macros for arbX_clear
#define Myarb_clear(a)                  arbX_clear ((a), 1)
#define arb1f_clear(a)                  arbX_clear ((a), 1)
#define arb2f_clear(a)                  arbX_clear ((a), 2)
#define arb4f_clear(a)                  arbX_clear ((a), 4)
#define arb8f_clear(a)                  arbX_clear ((a), 8)
#endif

#ifdef DEBUG
// Define macros for arbX_set
#define arb1f_set                       arb_set
#define arb2f_set(a,b)                  arb2_set ((a), (b))
#define arb4f_set(a,b)                  arb4_set ((a), (b))
#define arb8f_set(a,b)                  arb8_set ((a), (b))
#else
// Define macros for arbX_set
#define arb1f_set                       arb_set
#define arb2f_set(a,b)                  arbX_set ((a), (b), 2)
#define arb4f_set(a,b)                  arbX_set ((a), (b), 4)
#define arb8f_set(a,b)                  arbX_set ((a), (b), 8)
#endif

// Define pseudo-macro for arb_zero_p
#define arb_zero_p(a)                   arb_is_zero(a)

// Define pseudo-macro for arb_sign
#define arb_sign(a)                     (arb_is_zero (a) ? 0 : (arb_is_nonnegative (a) - arb_is_negative    (a)))
////ine arb_sign(a)                     (arb_is_zero (a) ? 0 : (arb_is_positive    (a) - arb_is_nonpositive (a)))

// Define pseudo-macro for arb_inf_p
#define arb_inf_p(a)                    arf_is_inf (arb_midref (a))

// Define pseudo-macro for arb_nan_p
#define arb_nan_p(a)                    arf_is_nan (arb_midref (a))

// Define pseudo-macro for arb_integer_p
#define arb_integer_p(a)                arb_is_int (a)

// Define pseudo-macro for arb_set_nan
#define arb_set_nan(a)                  arb_indeterminate (a)

// Define pseudo-macro for arb_get_prec
#define arb_get_prec(a)                 arb_rel_accuracy_bits (a)

// Define pseudo-macro for arb_cmp
#define arb_cmp(a,b)                    arf_cmp (arb_midref (a), arb_midref (b))
#define arb_cmp_si(a,b)                 arf_cmp_si (arb_midref (a), (b))
#define arb_cmp_ui(a,b)                 arf_cmp_ui (arb_midref (a), (b))

#define ARB_PREC_FPC                    ((mp_limb_signed_t) GetQuadFPC ())
#define ARB_PREC_FPC_PTD                ((mp_limb_signed_t) GetQuadFPC_PTD (lpMemPTD))

#ifndef DEBUG
#define ArbOut(a,b)
#define RatOut(a,b)
#define VfpOut(a,b)
#endif

// Define macros for detecting permutation vectors
#define IsPermVector0(lpHeader)         (((lpHeader) NE NULL) && (lpHeader)->PV0)
#define IsPermVector1(lpHeader)         (((lpHeader) NE NULL) && (lpHeader)->PV1)
#define IsPermVector(lpHeader)          (IsPermVector0 (lpHeader) || IsPermVector1 (lpHeader))

// Define macro for detecting All2s arrays
#define IsAll2s(lpHeader)               (((lpHeader) NE NULL) && (lpHeader)->All2s)

// Define macro for detecting scalars
#define IsScalar(ArrRank)               ((ArrRank) EQ 0)

// Define macro for detecting vectors
#define IsVector(ArrRank)               ((ArrRank) EQ 1)

// Define macro for detecting matrices
#define IsMatrix(ArrRank)               ((ArrRank) EQ 2)

// Define macro for detecting matrices or higher rank arrays
#define IsMultiRank(ArrRank)            ((ArrRank) >  1)

// Define macro for detecting rank 3 or higher rank arrays
#define IsRank3P(ArrRank)               ((ArrRank) >  2)

// Define macro for detecting empty arrays
#define IsEmpty(ArrNELM)                ((ArrNELM) EQ 0)

// Define macro for detecting singletons
#define IsSingleton(ArrNELM)            ((ArrNELM) EQ 1)

// Define macro for detecting multiple element arrays
#define IsMultiNELM(ArrNELM)            ((ArrNELM) >  1)

// Define macro for detecting singleton vectors
#define IsVectorSing(ArrNELM,ArrRank)   (IsSingleton (ArrNELM) && IsVector (ArrRank))

// Define macro for detecting simple scalars
#define IsSimpleScalar(ArrType,ArrRank) (IsSimple (ArrType) && IsScalar (ArrRank))

// Define macro for detecting zero dimensions
#define IsZeroDim(ArrDim)               ((ArrDim) EQ 0)

// Define macro for detecting unit dimensions
#define IsUnitDim(ArrDim)               ((ArrDim) EQ 1)

// Define macro for detecting length > 1 dimensions
#define IsMultiDim(ArrDim)              ((ArrDim) >  1)

// Define macro for detecting simple Boolean value
#define IsBooleanValue(Val)             ((Val) EQ 0 || (Val) EQ 1)

// Define macro for detecting simple Boolean RAT
#define IsBooleanRat(Rat)               (IsMpq0 (Rat) || IsMpq1 (Rat))

// Define macro for detecting simple Boolean VFP
#define IsBooleanVfp(Vfp)               (IsMpf0 (Vfp) || IsMpf1 (Vfp))

// Define macro for detecting simple Boolean ARB
#define IsBooleanArb(Arb)               (IsArb0 (Arb) || IsArb1 (Arb))

// Define macro for detecting an Integer 0
#define IsMpz0(Int)                     (mpz_cmp_si (Int, 0) EQ 0)

// Define macro for detecting an Integer 1
#define IsMpz1(Int)                     (mpz_cmp_si (Int, 1) EQ 0)

// Define macro for detecting an Rat 0
#define IsMpq0(Rat)                     (mpq_cmp_si (Rat, 0, 1) EQ 0)

// Define macro for detecting an Rat 1
#define IsMpq1(Rat)                     (mpq_cmp_si (Rat, 1, 1) EQ 0)

// Define macro for detecting a valid mpq_* var
#define IsMpqValid(a)                   ((a)->_mp_num._mp_d NE NULL)

// Define macro for detecting an Variable FP 0
#define IsMpf0(Vfp)                     (mpfr_cmp_si (Vfp, 0) EQ 0)

// Define macro for detecting an Variable FP 1
#define IsMpf1(Vfp)                     (mpfr_cmp_si (Vfp, 1) EQ 0)

// Define macro for detecting a valid mpfr_* var
#define IsMpfValid(a)                   ((a)->_mp_d NE NULL)

// Define macro for detecting an intermediate list
#define IsTknIntList(Tkn)               ((Tkn)->tkFlags.TknType EQ TKT_LISTINT)

// Define macro for detecting a Fill Jot token
#define IsTknFillJot(Tkn)               ((Tkn)->tkFlags.TknType EQ TKT_FILLJOT)

// Define macro for detecting a Function Array
#define IsTknFcnArray(Tkn)              ((Tkn)->tkFlags.TknType EQ TKT_FCNARRAY)

// Macro to skip over the TOKEN_HEADER
#define TokenBaseToStart(base)  (LPTOKEN) (ByteAddr (base, sizeof (TOKEN_HEADER)))

// Macros to skip from the variable array base to either the dimensions or the data
#define VarArrayBaseToDim(lpMem)          (LPAPLDIM) (((LPCHAR) (lpMem)) + sizeof (VARARRAY_HEADER)                              )
#define VarArrayDataFmBase(lpMem)         (LPVOID)   (((LPCHAR) (lpMem)) + sizeof (VARARRAY_HEADER) + sizeof (APLDIM) * (((LPVARARRAY_HEADER) lpMem)->Rank))
#define VarArrayDimToData(lpMem,aplRank)  (LPVOID)   (((LPCHAR) (lpMem))                            + sizeof (APLDIM) * (aplRank))

// Macro to skip from the function array base to the data
#define FcnArrayBaseToData(lpMem) (LPVOID)   (((LPCHAR) (lpMem)) + sizeof (FCNARRAY_HEADER))

// Macro to skip from the named array base to the data
#define VarNamedBaseToData(lpMem) (LPVOID)   (((LPCHAR) (lpMem)) + sizeof (VARNAMED_HEADER))

// Macro to skip from the list array base to the data
#define LstArrayBaseToData(lpMem) (LPVOID)   (((LPCHAR) (lpMem)) + sizeof (LSTARRAY_HEADER))

// Macros to clear the low-order bits of either an LPSYMENTRY,
//   or HGLOBAL (luckily, both types of ptrs are the same size).
// These macros come in either direct (Dir) or indirect (Ind) form
#define ClrPtrTypeDir(lpMem)        (LPVOID)     ((Assert (!IsPtrNullDir (lpMem)), (~PTRTYPE_MASK) &  (HANDLE_PTR  ) (lpMem)))
#define ClrPtrTypeInd(lpMem)        (LPVOID)     ((Assert (!IsPtrNullInd (lpMem)), (~PTRTYPE_MASK) & *(HANDLE_PTR *) (lpMem)))

// Macro to extract the low-order bits of a memory ptr used
//   to distinguish between the various pointer types.
// NOTE:  THIS MACRO CALLS ITS ARGUMENT *TWICE*, HENCE IT WILL WORK DIFFERENTLY
//        IN THE DEBUG VERSION FROM THE NON-DEBUG VERSION IF THE ARGUMENT HAS
//        ANY SIDE EFFECTS SUCH AS PRE- OR POST-INCREMENT/DECREMENT, OR THE LIKE.
#define GetPtrTypeDir(lpMem)        (BYTE)       (Assert (IsValidPtr (lpMem, sizeof (lpMem)) && !IsPtrNullDir (lpMem)), (PTRTYPE_MASK   &  (HANDLE_PTR  ) (lpMem)))
#define GetPtrTypeInd(lpMem)        (BYTE)       (Assert (IsValidPtr (lpMem, sizeof (lpMem)) && !IsPtrNullInd (lpMem)), (PTRTYPE_MASK   & *(HANDLE_PTR *) (lpMem)))

// Macro to create a masked LPSYMENTRY
#define MakePtrTypeSym(lpMem)       (LPSYMENTRY) (Assert (IsValidPtr (lpMem, sizeof (lpMem)) && !IsPtrNullDir (lpMem)), PTRTYPE_STCONST |  (HANDLE_PTR  ) (lpMem))

// Macro to create a masked HGLOBAL
#define MakePtrTypeGlb(lpMem)       (HGLOBAL)    (Assert (IsValidPtr (lpMem, sizeof (lpMem)) && !IsPtrNullDir (lpMem)), PTRTYPE_HGLOBAL |  (HANDLE_PTR  ) (lpMem))

// Macro to copy direct and indirect ptrs, incrementing the reference count
#define CopySymGlbDirAsGlb(hGlb)                        CopySymGlbDir_PTB (MakePtrTypeGlb (hGlb))
#define CopySymGlbNumDirAsGlb(hGlb,aplType,lptkFunc)    CopySymGlbNumDir_PTB (MakePtrTypeGlb (hGlb), aplType, lptkFunc)
#define CopySymGlbInd_PTB(lpSymGlb)                     CopySymGlbDir_PTB (*(LPAPLNESTED) lpSymGlb)

// Macro to convert a number into a global handle
#define MakeGlbFromPtr(a)                  (*(HGLOBAL *)   (a))
#define MakeGlbFromVal(a)                  MakeGlbFromPtr(&(a))

// Macros to check on ptr NULL
#define IsPtrNullDir(lpMem)                                             ( (lpMem)  EQ NULL)
#define IsPtrNullInd(lpMem)         (IsPtrNullDir (lpMem) || (*(LPVOID *) (lpMem)) EQ NULL)

// Note that some of the following macros depend upon
//   the ordering of the enum IMM_TYPES in <symtab.h>
#define IsImmBool(a)                ((a) EQ IMMTYPE_BOOL)
#define IsImmInt(a)                 (IMMTYPE_ERROR < (a) && (a) < IMMTYPE_FLOAT)
#define IsImmNum(a)                 (IMMTYPE_ERROR < (a) && (a) < IMMTYPE_CHAR)
#define IsImmFlt(a)                 ((a) EQ IMMTYPE_FLOAT)
#define IsImmChr(a)                 ((a) EQ IMMTYPE_CHAR)
#define IsImmRat(a)                 ((a) EQ IMMTYPE_RAT)
#define IsImmVfp(a)                 ((a) EQ IMMTYPE_VFP)
#define IsImmArb(a)                 ((a) EQ IMMTYPE_ARB)
#define IsImmGlbNum(a)              (IMMTYPE_RAT <= (a) && (a) <= IMMTYPE_BA8F)
#define IsImmErr(a)                 ((a) EQ IMMTYPE_ERROR)
#define IsImmFcn(a)                 ((a) EQ IMMTYPE_PRIMFCN)
#define IsImmOpr(a)                 (IMMTYPE_PRIMOP1 <= (a) && (a) <= IMMTYPE_PRIMOP3)
#define IsImmFop(a)                 (IsImmFcn (a) || IsImmOpr (a))


// The enum NAME_TYPES in <symtab.h> is constructed to allow
//  the following macros to be used.
#define IsNameTypeFn(a)             ((a) &  NAMETYPEMASK_FN                   )
#define IsNameTypeOp(a)             ((a) &                    NAMETYPEMASK_OP )
#define IsNameTypeFnOp(a)           ((a) & (NAMETYPEMASK_FN | NAMETYPEMASK_OP))
#define IsNameTypeVar(a)            ((a) EQ NAMETYPE_VAR)
#define IsNameTypeName(a)           (IsNameTypeVar (a) || IsNameTypeFnOp (a))

#ifdef DEBUG
  #define GetSignatureMem(a)          (Assert (((LPHEADER_SIGNATURE) (a))->nature NE 0), ((LPHEADER_SIGNATURE) (a))->nature)
#else
  #define GetSignatureMem(a)          (((LPHEADER_SIGNATURE) (a))->nature)
#endif

#define GetImmTypeFcn(a)            IMMTYPE_PRIMFCN

#define NextBooleanByte(lpMem,uRes) ((LPAPLBOOL) lpMem)[((UINT) (uRes)) >> LOG2NBIB]
#define GetNextBoolean(lpMem,uRes)  (BIT0 & (NextBooleanByte (lpMem, uRes) >> (MASKLOG2NBIB & (UINT) (uRes))))
#define ShiftBoolean(value,uRes)    (value) << (MASKLOG2NBIB & (UINT) (uRes))
#define GetNextChar8(a,t,c)         ((LPUINT8   ) a)[c]
#define GetNextChar16(a,t,c)        ((LPUINT16  ) a)[c]
#define GetNextChar32(a,t,c)        ((LPUINT32  ) a)[c]
#define GetNextInt8(a,t,c)          ((LPINT8    ) a)[c]
#define GetNextInt16(a,t,c)         ((LPINT16   ) a)[c]
#define GetNextInt32(a,t,c)         ((LPINT32   ) a)[c]
#define GetNextInt64(a,t,c)         ((LPINT64   ) a)[c]
#define GetNextFlt32(a,t,c)         ((LPAPLFLT32) a)[c]
#define GetNextFlt64(a,t,c)         ((LPAPLFLT64) a)[c]

// Macro to determine if the System Name is one of []A and friends
#define IsQuadA(a)                  (strchrW (L"aA����������", (a)) NE NULL)

// Macros to get countof for arrays and constant strings
#define countof(a)                  (sizeof (a) / sizeof ((a)[0]))
#define strcountof(a)               (countof (a) - 1)
#define strsizeof(a)                (sizeof (a) - 1)

// Wide char versions
#define CopyMemoryW(dst,src,len)    CopyMemory ((dst), (src), (len) * sizeof (WCHAR))
#define MoveMemoryW(dst,src,len)    MoveMemory ((dst), (src), (len) * sizeof (WCHAR))
// Define macro to copy a entire var to a named var
#define CopyAll(dst,src)            CopyMemory ((dst), (src), sizeof (*dst))

// Macros for returning a result from a Dialog
#define DlgReturn(hDlg,lRet)        SetWindowLongPtr (hDlg, DWLP_MSGRESULT, lRet);  \
                                    return lRet;
#define DlgMsgPass(hDlg)            DlgReturn (hDlg, DLG_MSGPASS)   // Pass this msg on to the next handler
#define DlgMsgDone(hDlg)            DlgReturn (hDlg, DLG_MSGDONE)   // Do not pass this msg on to the next handler
#define DLG_MSGPASS                 FALSE           // Pass this Dlg msg on to the next handler
#define DLG_MSGDONE                 TRUE            // We handled this Dlg msg
#define DLG_MSGDEFFOCUS             TRUE            // In WM_INITDIALOG, set keyboard focus to wParam, DWLP_MSGRESULT ignored
#define DLG_MSGNODEFFOCUS           FALSE           // ...               do not set focus

// Define macro for detecting floating point infinity
#define _isinf(a)           (!_finite (a) && !_isnan (a))
#define IsFltInfinity(a)    _isinf (a)
#define IsFltPosInfinity(a) (!_finite (a) && !_isnan (a) && !SIGN_APLFLOAT (a))
#define IsFltNegInfinity(a) (!_finite (a) && !_isnan (a) &&  SIGN_APLFLOAT (a))
#define IsFltNaN(a)         (!_finite (a) &&  _isnan (a))

// Define macro for detecting Rational or VFP infinity
#define IsMpzInfinity(a)    (mpz_inf_p (a))
#define IsMpqInfinity(a)    (mpq_inf_p (a))
#define IsMpfInfinity(a)    (mpfr_inf_p (a))
#define IsMpfNaN(a)         (mpfr_nan_p (a))

// Define macro for inverting VFP numbers
#define mpfr_inv(rop,op,rnd)    mpfr_si_div (rop, 1, op, rnd)

// Define macros for converting 64-bit integers to MPFR
#define mpfr_set_sx     mpfr_set_sj
#define mpfr_set_ux     mpfr_set_uj

// Define macros for extracting the left and right operands from operators
#define GetMonLftOper(lpYYFcnStrOpr,lptkAxisOpr)                ((lpYYFcnStrOpr->TknCount <= (UINT) (1 + (lptkAxisOpr NE NULL)                          )) ? NULL : (&lpYYFcnStrOpr[1 + (lptkAxisOpr NE NULL)                          ]))
#define GetDydLftOper(lpYYFcnStrOpr,lptkAxisOpr,lpYYFcnStrRht)  ((lpYYFcnStrOpr->TknCount <= (UINT) (1 + (lptkAxisOpr NE NULL) + lpYYFcnStrRht->TknCount)) ? NULL : (&lpYYFcnStrOpr[1 + (lptkAxisOpr NE NULL) + lpYYFcnStrRht->TknCount]))
#define GetDydRhtOper(lpYYFcnStrOpr,lptkAxisOpr)                GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr)

// Define macro to determine if a token is a valid result
#define IsTknValid(a)                               \
    ((a).tkFlags.TknType NE TKT_UNUSED              \
     && ((a).tkFlags.TknType NE TKT_VARNAMED        \
      || (a).tkData.tkSym->stFlags.Value EQ TRUE))

// Define macro to free and YYFree a result, and set to NULL
#define FreeYYRes(lpYYRes)                          \
    if ((lpYYRes) NE NULL)                          \
    {                                               \
        FreeResult (lpYYRes);                       \
        YYFree (lpYYRes);                           \
        (lpYYRes) = NULL;                           \
    } /* End IF */

// Define macro to free (and set to NULL) a global name
#define FreeGlbName(hGlb)                           \
    if ((hGlb) NE NULL)                             \
    {                                               \
        DbgGlobalFree (hGlb); (hGlb) = NULL;        \
    } /* End IF */

// Define macro to unlock and free (and set to NULL) a global name and ptr
#define UnlFreeGlbName(hGlb,lpMem)                  \
    if ((hGlb) NE NULL)                             \
    {                                               \
        if ((lpMem) NE NULL)                        \
        {                                           \
            /* We no longer need this ptr */        \
            MyGlobalUnlock (hGlb); (lpMem) = NULL;  \
        } /* End IF */                              \
                                                    \
        /* We no longer need this storage */        \
        DbgGlobalFree (hGlb); (hGlb) = NULL;        \
    } /* End IF */

// Define macros that expand to another function
#define FloatToAplint_CT(a,b,c)     (_FloatToAplint_CT (a, b, c, FALSE))

#define mpq_get_sctsx(a,b)          _mpq_get_ctsx (a, SYS_CT, b, TRUE)
#define mpq_get_ctsx(a,b,c)         _mpq_get_ctsx (a, b, c, FALSE)
#define mpfr_get_ctsx(a,b,c)        _mpfr_get_ctsx (a, b, c, FALSE)

#define arb_get_sctsx(a,b)          _arb_get_ctsx (a, SYS_CT, b, TRUE)
#define arb_get_ctsx(a,b,c)         _arb_get_ctsx (a, b, c, FALSE)

// Define macro for detecting a character in the range for Output Debugging
#define IsOutDbg(a)             ((L' ' > (unsigned) a) || (a) EQ UTF16_REPLACEMENT0000)

// Define macro to get a ptr to the global memory
#define GetGlbMem(lpMemHdr,lpaplLongest)                \
    /* If the right arg is a global, ...              */\
    (lpMemHdr NE NULL) ? VarArrayDataFmBase (lpMemHdr)  \
                       : lpaplLongest;


//***************************************************************************
//  End of File: macros.h
//***************************************************************************