//***************************************************************************
//  NARS2000 -- Primitive Function Special Header
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

// Call this function for each monadic primitive function
typedef LPPL_YYSTYPE PRIMFN_MON
    (LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis,                      // Ptr to axis token (may be NULL)
     struct tagPrimSpec *lpPrimSpec);       // Ptr to local PRIMSPEC

typedef PRIMFN_MON *LPPRIMFN_MON;

// Call this function for each dyadic primitive function
typedef LPPL_YYSTYPE PRIMFN_DYD
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis,                      // Ptr to axis token (may be NULL)
     struct tagPrimSpec *lpPrimSpec);       // Ptr to local PRIMSPEC

typedef PRIMFN_DYD *LPPRIMFN_DYD;

typedef BOOL PRIMFN_DYD_SNvSN
    (LPPL_YYSTYPE lpYYRes,                  // Ptr to the result

     LPTOKEN      lptkLftArg,               // Ptr to left arg token
     LPTOKEN      lptkFunc,                 // Ptr to function token
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token

     HGLOBAL      hGlbLft,                  // Handle to left arg
     HGLOBAL      hGlbRht,                  // ...       right ...
     HGLOBAL     *lphGlbRes,                // Ptr to handle to result

     LPVOID       lpMemLft,                 // Points to Sig.nature
     LPVOID       lpMemRht,                 // ...

     LPAPLUINT    lpMemAxisHead,            // Ptr to axis values, fleshed out
     LPAPLUINT    lpMemAxisTail,            // Ptr to grade up of AxisHead

     APLRANK      aplRankLft,               // Left arg rank
     APLRANK      aplRankRht,               // Right ...
     APLRANK      aplRankRes,               // Result ...

     APLSTYPE     aplTypeLft,               // Left arg storage type
     APLSTYPE     aplTypeRht,               // Right ...
     APLSTYPE     aplTypeRes,               // Result ...

     APLNELM      aplNELMLft,               // Left arg NELM
     APLNELM      aplNELMRht,               // Right ...
     APLNELM      aplNELMRes,               // Result ...
     APLNELM      aplNELMAxis,              // Axis ...
     struct tagPrimSpec *lpPrimSpec);       // Ptr to local PRIMSPEC

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
typedef BOOL APARESULT_MON
        (LPPL_YYSTYPE lpYYRes,              // Ptr to the result (may be NULL)

         LPTOKEN      lptkFunc,             // Ptr to function token

         HGLOBAL      hGlbRht,              // HGLOBAL of right arg
         HGLOBAL     *lphGlbRes,            // ...        result

         APLRANK      aplRankRht,           // Right arg rank

         struct tagPrimSpec *lpPrimSpec);   // Ptr to local PRIMSPEC

typedef APARESULT_MON *LPAPARESULT_MON;

// Call this function if the dyadic result is APA
typedef BOOL APARESULT_DYD
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
         struct tagPrimSpec *lpPrimSpec);   // Ptr to local PRIMSPEC

typedef APARESULT_DYD *LPAPARESULT_DYD;


typedef APLBOOL  BISB   (APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISI   (APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISF   (APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISC   (APLCHAR , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISI   (APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISF   (APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISC   (APLCHAR , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISI   (APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISF   (APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISC   (APLCHAR , struct tagPrimSpec *lpPrimSpec);


typedef APLBOOL  BISBVB (APLBOOL , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISBVI (APLBOOL , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISBVF (APLBOOL , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISBVC (APLBOOL , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLBOOL  BISIVB (APLINT  , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISIVI (APLINT  , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISIVF (APLINT  , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISIVC (APLINT  , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLBOOL  BISFVB (APLFLOAT, APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISFVI (APLFLOAT, APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISFVF (APLFLOAT, APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISFVC (APLFLOAT, APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLBOOL  BISCVB (APLCHAR , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISCVI (APLCHAR , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISCVF (APLCHAR , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLBOOL  BISCVC (APLCHAR , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLINT   IISBVB (APLBOOL , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISBVI (APLBOOL , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISBVF (APLBOOL , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISBVC (APLBOOL , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLINT   IISIVB (APLINT  , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISIVI (APLINT  , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISIVF (APLINT  , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISIVC (APLINT  , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLINT   IISFVB (APLFLOAT, APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISFVI (APLFLOAT, APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISFVF (APLFLOAT, APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISFVC (APLFLOAT, APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLINT   IISCVB (APLCHAR , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISCVI (APLCHAR , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISCVF (APLCHAR , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLINT   IISCVC (APLCHAR , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLFLOAT FISBVB (APLBOOL , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISBVI (APLBOOL , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISBVF (APLBOOL , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISBVC (APLBOOL , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLFLOAT FISIVB (APLINT  , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISIVI (APLINT  , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISIVF (APLINT  , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISIVC (APLINT  , APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLFLOAT FISFVB (APLFLOAT, APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISFVI (APLFLOAT, APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISFVF (APLFLOAT, APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISFVC (APLFLOAT, APLCHAR , struct tagPrimSpec *lpPrimSpec);

typedef APLFLOAT FISCVB (APLCHAR , APLBOOL , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISCVI (APLCHAR , APLINT  , struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISCVF (APLCHAR , APLFLOAT, struct tagPrimSpec *lpPrimSpec);
typedef APLFLOAT FISCVC (APLCHAR , APLCHAR , struct tagPrimSpec *lpPrimSpec);


// General structure passed to all primitive functions
typedef struct tagPrimSpec
{
    // Monadic functions
    LPPRIMFN_MON        PrimFnMon_EM_YY;    // Ptr to monadic primitive function
    LPSTORAGE_TYPE_MON  StorageTypeMon;     // ...            storage type ...
    LPAPARESULT_MON     ApaResultMon_EM;    // ...            APA result ...

    BISB               *BisB;               // Monadic B {is} B
    BISI               *BisI;               // ...            I
    BISF               *BisF;               // ...            F

////IISB               *IisB;               // Handled via type promotion (to IisI)
    IISI               *IisI;               // Monadic I {is} I
    IISF               *IisF;               // ...            F

////FISB               *FisB;               // Handled via type promotion (to FisF)
    FISI               *FisI;               // Monadic F {is} I
    FISF               *FisF;               // ...            F

    // Dyadic functions
    LPPRIMFN_DYD        PrimFnDyd_EM_YY;    // Ptr to dyadic primitive function
    LPSTORAGE_TYPE_DYD  StorageTypeDyd;     // ...           storage type ...
    LPAPARESULT_DYD     ApaResultDyd_EM;    // ...           APA result ...

    BISBVB             *BisBvB;             // Dyadic B {is} B vs B
    BISIVI             *BisIvI;             // ...           I vs I
    BISFVF             *BisFvF;             // ...           F vs F
    BISCVC             *BisCvC;             // ...           C vs C

////IISBVB             *IisBvB;             // Handled via type promotion (to IisIvI)
    IISIVI             *IisIvI;             // Dyadic I {is} I vs I
    IISFVF             *IisFvF;             // ...           F vs F

////FISBVB             *FisBvB;             // Handled via type promotion (to FisFvF)
    FISIVI             *FisIvI;             // Dyadic F {is} I vs I
    FISFVF             *FisFvF;             // ...           F vs F
    APLUINT             QuadRL;             // []RL for atomicity
} PRIMSPEC, *LPPRIMSPEC;


//***************************************************************************
//  End of File: primspec.h
//***************************************************************************
