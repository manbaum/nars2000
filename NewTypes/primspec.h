//***************************************************************************
//  NARS2000 -- Primitive Function Special Header
//***************************************************************************

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

// Define incomplete definition of a struc defined in full later
struct tagPRIMSPEC;

// Call this function for each monadic primitive function
typedef LPPL_YYSTYPE PRIMFN_MON
    (LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis,                      // Ptr to axis token (may be NULL)
     struct tagPRIMSPEC *lpPrimSpec);       // Ptr to local PRIMSPEC

typedef PRIMFN_MON *LPPRIMFN_MON;

// Call this function for each dyadic primitive function
typedef LPPL_YYSTYPE PRIMFN_DYD
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis,                      // Ptr to axis token (may be NULL)
     struct tagPRIMSPEC *lpPrimSpec);       // Ptr to local PRIMSPEC

typedef PRIMFN_DYD *LPPRIMFN_DYD;

typedef UBOOL PRIMFN_DYD_SNvSN
    (LPPL_YYSTYPE      lpYYRes,             // Ptr to the result

     LPTOKEN           lptkLftArg,          // Ptr to left arg token
     LPTOKEN           lptkFunc,            // Ptr to function token
     LPTOKEN           lptkRhtArg,          // Ptr to right arg token

     HGLOBAL           hGlbLft,             // Handle to left arg
     HGLOBAL           hGlbRht,             // ...       right ...
     HGLOBAL          *lphGlbRes,           // Ptr to handle to result

     LPVARARRAY_HEADER lpMemHdrLft,         // Ptr to left arg header
     LPVARARRAY_HEADER lpMemHdrRht,         // ...    right ...

     LPAPLUINT         lpMemAxisHead,       // Ptr to axis values, fleshed out by CheckAxis_EM (may be NULL)
     LPAPLUINT         lpMemAxisTail,       // Ptr to grade up of AxisHead

     APLRANK           aplRankLft,          // Left arg rank
     APLRANK           aplRankRht,          // Right ...
     APLRANK           aplRankRes,          // Result ...

     APLSTYPE          aplTypeLft,          // Left arg storage type
     APLSTYPE          aplTypeRht,          // Right ...
     APLSTYPE          aplTypeRes,          // Result ...

     APLNELM           aplNELMLft,          // Left arg NELM
     APLNELM           aplNELMRht,          // Right ...
     APLNELM           aplNELMRes,          // Result ...
     APLNELM           aplNELMAxis,         // Axis ...

     UBOOL             bLftIdent,           // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL             bRhtIdent,           // ...                         right ...

     struct tagPRIMSPEC *lpPrimSpec);       // Ptr to local PRIMSPEC

typedef PRIMFN_DYD_SNvSN *LPPRIMFN_DYD_SNvSN;

// Call this function to determine the storage type of the monadic result
typedef APLSTYPE STORAGE_TYPE_MON
        (APLNELM    aplNELMRht,             // Right arg NELM
         LPAPLSTYPE lpaplTypeRht,           // Ptr to right arg storage type
         LPTOKEN    lptkFunc);              // Ptr to function token

typedef STORAGE_TYPE_MON *LPSTORAGE_TYPE_MON;

// Call this function to determine the storage type of the dyadic result
typedef APLSTYPE STORAGE_TYPE_DYD
        (APLNELM    aplNELMLft,             // Left arg NELM
         LPAPLSTYPE lpaplTypeLft,           // Ptr to left arg storage type
         LPTOKEN    lptkFunc,               // Ptr to function token
         APLNELM    aplNELMRht,             // Right arg NELM
         LPAPLSTYPE lpaplTypeRht);          // Ptr to right arg storage type

typedef STORAGE_TYPE_DYD *LPSTORAGE_TYPE_DYD;

// Call this function if the monadic result is APA
typedef UBOOL APARESULT_MON
        (LPPL_YYSTYPE lpYYRes,              // Ptr to the result (may be NULL)

         LPTOKEN      lptkFunc,             // Ptr to function token

         HGLOBAL      hGlbRht,              // HGLOBAL of right arg
         HGLOBAL     *lphGlbRes,            // ...        result

         APLRANK      aplRankRht,           // Right arg rank

         struct tagPRIMSPEC *lpPrimSpec);   // Ptr to local PRIMSPEC

typedef APARESULT_MON *LPAPARESULT_MON;

// Call this function if the dyadic result is APA
typedef UBOOL APARESULT_DYD
        (LPPL_YYSTYPE lpYYRes,              // Ptr to the result (may be NULL)

         LPTOKEN      lptkFunc,             // Ptr to function token

         HGLOBAL      hGlbLft,              // HGLOBAL of left arg (may be NULL if simple)
         HGLOBAL      hGlbRht,              // ...        right ...
         HGLOBAL     *lphGlbRes,            // ...        result

         APLRANK      aplRankLft,           // Left arg rank
         APLRANK      aplRankRht,           // Right ...

         APLNELM      aplNELMLft,           // Left arg NELM
         APLNELM      aplNELMRht,           // Right ...

         APLINT       aplInteger,           // The integer from the simple side
         struct tagPRIMSPEC *lpPrimSpec);   // Ptr to local PRIMSPEC

typedef APARESULT_DYD *LPAPARESULT_DYD;


// Monadic Boolean result functions
typedef APLBOOL  BISB               (APLBOOL           , struct tagPRIMSPEC *);
typedef APLBOOL  BISI               (APLINT            , struct tagPRIMSPEC *);
typedef APLBOOL  BISF               (APLFLOAT          , struct tagPRIMSPEC *);

// Monadic Boolean chunk functions
typedef APLB64   B64ISB64           (APLB64            , struct tagPRIMSPEC *);
typedef APLB32   B32ISB32           (APLB32            , struct tagPRIMSPEC *);
typedef APLB16   B16ISB16           (APLB16            , struct tagPRIMSPEC *);
typedef APLB08   B08ISB08           (APLB08            , struct tagPRIMSPEC *);

// Dyadic Boolean chunk functions
typedef APLB64   B64ISB64VB64       (APLB64  , APLB64  , struct tagPRIMSPEC *);
typedef APLB32   B32ISB32VB32       (APLB32  , APLB32  , struct tagPRIMSPEC *);
typedef APLB16   B16ISB16VB16       (APLB16  , APLB16  , struct tagPRIMSPEC *);
typedef APLB08   B08ISB08VB08       (APLB08  , APLB08  , struct tagPRIMSPEC *);


// General structure passed to all primitive functions
typedef struct tagPRIMSPEC
{
    // Monadic functions
    LPPRIMFN_MON        PrimFnMon_EM_YY;    // Ptr to monadic primitive function
    LPSTORAGE_TYPE_MON  StorageTypeMon;     // ...            storage type ...
    LPAPARESULT_MON     ApaResultMon_EM;    // ...            APA result ...

    // Monadic Boolean result functions
    BISB               *BisB;               // Monadic B {is} B
    BISI               *BisI;               // ...            I         (Tilde)
    BISF               *BisF;               // ...            F          ...

    // Monadic non-Boolean result functions (indexable)
    ATISAT             *IisI;               // Monadic    I {is}    I
    ATISAT             *IisF;               // ...        I         F
    ATISAT             *FisI;               // ...        F         I
    ATISAT             *FisF;               // ...        F         F
    ATISAT             *RisR;               // ...        R         R
    ATISAT             *VisR;               // ...        V         R
    ATISAT             *VisV;               // ...        V         V

    ATISAT             *HC2IisHC2I;         // Monadic HC2I {is} HC2I
    ATISAT             *HC2IisHC2F;         // ...        I         F
    ATISAT             *HC2FisHC2I;         // ...        F         I
    ATISAT             *HC2FisHC2F;         // ...        F         F
    ATISAT             *HC2RisHC2R;         // ...        R         R
    ATISAT             *HC2VisHC2R;         // ...        V         R
    ATISAT             *HC2VisHC2V;         // ...        V         V

    ATISAT             *HC4IisHC4I;         // Monadic HC4I {is} HC4I
    ATISAT             *HC4IisHC4F;         // ...        I         F
    ATISAT             *HC4FisHC4I;         // ...        F         I
    ATISAT             *HC4FisHC4F;         // ...        F         F
    ATISAT             *HC4RisHC4R;         // ...        R         R
    ATISAT             *HC4VisHC4R;         // ...        V         R
    ATISAT             *HC4VisHC4V;         // ...        V         V

    ATISAT             *HC8IisHC8I;         // Monadic HC8I {is} HC8I
    ATISAT             *HC8IisHC8F;         // ...        I         F
    ATISAT             *HC8FisHC8I;         // ...        F         I
    ATISAT             *HC8FisHC8F;         // ...        F         F
    ATISAT             *HC8RisHC8R;         // ...        R         R
    ATISAT             *HC8VisHC8R;         // ...        V         R
    ATISAT             *HC8VisHC8V;         // ...        V         V

    // Monadic FLT/VFP result HC arg functions (indexable)
    ATISAT             *FisHC2I;            // Monadic    F      HC2I   (Stile)
    ATISAT             *FisHC2F;            // ...        F      HC2F    ...
    ATISAT             *VisHC2R;            // ...        V      HC2R    ...
    ATISAT             *VisHC2V;            // ...        V      HC2V    ...
    ATISAT             *FisHC4I;            // ...        F      HC4I    ...
    ATISAT             *FisHC4F;            // ...        F      HC4F    ...
    ATISAT             *VisHC4R;            // ...        V      HC4R    ...
    ATISAT             *VisHC4V;            // ...        V      HC4V    ...
    ATISAT             *FisHC8I;            // ...        F      HC8I    ...
    ATISAT             *FisHC8F;            // ...        F      HC8F    ...
    ATISAT             *VisHC8R;            // ...        V      HC8R    ...
    ATISAT             *VisHC8V;            // ...        V      HC8V    ...

    // Dyadic functions
    LPPRIMFN_DYD        PrimFnDyd_EM_YY;    // Ptr to dyadic primitive function
    LPSTORAGE_TYPE_DYD  StorageTypeDyd;     // ...           storage type ...
    LPAPARESULT_DYD     ApaResultDyd_EM;    // ...           APA result ...

    // Dyadic Boolean result functions (indexable)
    ATISATVAT          *BisBvB;             // Dyadic B {is} B vs B
    ATISATVAT          *BisIvI;             // ...           I vs I
    ATISATVAT          *BisFvF;             // ...           F vs F
    ATISATVAT          *BisCvC;             // ...           C vs C
    ATISATVAT          *BisHvH;             // ...           H vs H
    ATISATVAT          *BisRvR;             // ...           R vs R
    ATISATVAT          *BisVvV;             // ...           V vs V

    ATISATVAT          *BisHC2IvHC2I;       // Dyadic B {is} HC2I vs HC2I
    ATISATVAT          *BisHC2FvHC2F;       // ...           HC2F vs HC2F
    ATISATVAT          *BisHC2RvHC2R;       // ...           HC2R vs HC2R
    ATISATVAT          *BisHC2VvHC2V;       // ...           HC2V vs HC2V

    ATISATVAT          *BisHC4IvHC4I;       // Dyadic B {is} HC4I vs HC4I
    ATISATVAT          *BisHC4FvHC4F;       // ...           HC4F vs HC4F
    ATISATVAT          *BisHC4RvHC4R;       // ...           HC4R vs HC4R
    ATISATVAT          *BisHC4VvHC4V;       // ...           HC4V vs HC4V

    ATISATVAT          *BisHC8IvHC8I;       // Dyadic B {is} HC8I vs HC8I
    ATISATVAT          *BisHC8FvHC8F;       // ...           HC8F vs HC8F
    ATISATVAT          *BisHC8RvHC8R;       // ...           HC8R vs HC8R
    ATISATVAT          *BisHC8VvHC8V;       // ...           HC8V vs HC8V

    // Dyadic non-Boolean result functions (indexable)
    ATISATVAT          *IisIvI;             // Dyadic  I {is}     I vs     I
    ATISATVAT          *IisFvF;             // ...     I          F        F
    ATISATVAT          *FisIvI;             // ...     F          I        I
    ATISATVAT          *FisFvF;             // ...     F          F        F
    ATISATVAT          *RisRvR;             // ...     R          R        R
    ATISATVAT          *VisRvR;             // ...     V          R        R
    ATISATVAT          *VisVvV;             // ...     V          V        V

    ATISATVAT          *AT2IisAT2IvAT2I;    // Dyadic 2I {is} AT_2I vs AT_2I
    ATISATVAT          *AT2IisAT2FvAT2F;    // ...    2I         2F       2F
    ATISATVAT          *AT2FisAT2IvAT2I;    // ...    2F         2I       2I
    ATISATVAT          *AT2FisAT2FvAT2F;    // ...    2F         2F       2F
    ATISATVAT          *AT2RisAT2RvAT2R;    // ...    2R         2R       2R
    ATISATVAT          *AT2VisAT2RvAT2R;    // ...    2V         2R       2R
    ATISATVAT          *AT2VisAT2VvAT2V;    // ...    2V         2V       2V

    ATISATVAT          *AT4IisAT4IvAT4I;    // Dyadic 4I {is} AT_4I vs AT_4I
    ATISATVAT          *AT4IisAT4FvAT4F;    // ...    4I         4F       4F
    ATISATVAT          *AT4FisAT4IvAT4I;    // ...    4F         4I       4I
    ATISATVAT          *AT4FisAT4FvAT4F;    // ...    4F         4F       4F
    ATISATVAT          *AT4RisAT4RvAT4R;    // ...    4R         4R       4R
    ATISATVAT          *AT4VisAT4RvAT4R;    // ...    4V         4R       4R
    ATISATVAT          *AT4VisAT4VvAT4V;    // ...    4V         4V       4V

    ATISATVAT          *AT8IisAT8IvAT8I;    // Dyadic 8I {is} AT_8I vs AT_8I
    ATISATVAT          *AT8IisAT8FvAT8F;    // ...    8I         8F       8F
    ATISATVAT          *AT8FisAT8IvAT8I;    // ...    8F         8I       8I
    ATISATVAT          *AT8FisAT8FvAT8F;    // ...    8F         8F       8F
    ATISATVAT          *AT8RisAT8RvAT8R;    // ...    8R         8R       8R
    ATISATVAT          *AT8VisAT8RvAT8R;    // ...    8V         8R       8R
    ATISATVAT          *AT8VisAT8VvAT8V;    // ...    8V         8V       8V

    // Monadic Boolean chunk functions
    B64ISB64           *B64isB64;           // Monadic B64 {is} B64
    B32ISB32           *B32isB32;           // ...     B32 {is} B32
    B16ISB16           *B16isB16;           // ...     B16 {is} B16
    B08ISB08           *B08isB08;           // ...     B08 {is} B08

    // Dyadic Boolean chunk functions
    B64ISB64VB64       *B64isB64vB64;       // Dyadic B64 {is} B64 v B64
    B32ISB32VB32       *B32isB32vB32;       // ...    B32 {is} B32 v B32
    B16ISB16VB16       *B16isB16vB16;       // ...    B16 {is} B16 v B16
    B08ISB08VB08       *B08isB08vB08;       // ...    B08 {is} B08 v B08

    // Miscellaneous
    APLUINT             uQuadRL;            // []RL for atomicity
    UINT                bMonDimDemote:1,    // 00000001:  Allow dimension demotion for monadic scalar function
                        bDydDimDemote:1,    // 00000002:  ...                          dyadic  ...
                        bLclDimDemote:1,    // 00000004:  ...                          local   ...
                        eHCMul:3,           // 00000038:  Hypercomplex Multiplication Variant (see tagENUM_HCMUL)
                        :26;                // FFFFFFC0:  Available bits
} PRIMSPEC, *LPPRIMSPEC;

#define HC1IisHC1I          IisI
#define HC1IisHC1F          IisF
#define HC1FisHC1I          FisI
#define HC1FisHC1F          FisF
#define HC1RisHC1R          RisR
#define HC1VisHC1V          VisV
#define HC1IisHC1IvHC1I     IisIvI
#define HC1IisHC1FvHC1F     IisFvF
#define HC1FisHC1IvHC1I     FisIvI
#define HC1FisHC1FvHC1F     FisFvF
#define HC1RisHC1RvHC1R     RisRvR
#define HC1VisHC1VvHC1V     VisVvV

#define mphc1r_init         mpq_init
#define mphc1v_init0        mpfr_init0
#define Myhc1r_clear        Myq_clear
#define Myhc1v_clear        Myf_clear


//***************************************************************************
//  End of File: primspec.h
//***************************************************************************
